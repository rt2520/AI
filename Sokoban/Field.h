#ifndef FIELD_H
#define FIELD_H
#include "Square.h"
#include <vector>
#include <fstream>
#include <string>

using namespace std;

class Field
{
    public:
        Field(char* filename)
        {
            cout<<filename;
            int lines = 0;
            string line;
            int maxCols = 0;
            int currLine = 0;
            std::ifstream field_file(filename);
            if (field_file.is_open())
            {
                getline(field_file, line);
                while (getline(field_file, line))
                {
                    if (line.length() > maxCols)
                        maxCols = line.length();
                }
                field_file.close();
            }
            field_file.open(filename);
            cout<<endl;
            if (field_file.is_open())
            {
                getline(field_file, line);
                while (getline(field_file, line))
                {
                    cout<<line<<endl;
                    vector<Square> row;
                    for (int i = 0; i < line.length(); i++)
                    {
                        switch(line[i])
                        {
                            case ' ':
                                row.push_back(Square(currLine,i,EMPTY));
                                break;
                            case '#':
                                row.push_back(Square(currLine,i,WALL));
                                break;
                            case '.':
                                row.push_back(Square(currLine,i,GOAL));
                                m_goals.push_back(Point(currLine,i));
                                break;
                            case '@':
                                row.push_back(Square(currLine,i,EMPTY));
                                m_player = Point(currLine,i);
                                break;
                            case '+':
                                row.push_back(Square(currLine,i,GOAL));
                                m_player = Point(currLine,i);
                                m_goals.push_back(Point(currLine,i));
                                break;
                            case '$':
                                row.push_back(Square(currLine,i,EMPTY));
                                m_boxes.push_back(Point(currLine,i));
                                break;
                            case '*':
                                row.push_back(Square(currLine,i,GOAL));
                                m_goals.push_back(Point(currLine,i));
                                m_boxes.push_back(Point(currLine,i));
                                break;
                            default:
                                cerr<<"Invalid character in input file at"<<currLine+1<<","<<i<<endl;
                        }
                    }
                    for (int i=0;i<maxCols;i++)
                    {
                        if (row[i].getType() == WALL)
                            break;
                        row[i].setType(WALL);
                    }
                    while (row.size() < maxCols)
                        row.push_back(Square(currLine,row.size(),WALL));
                    m_map.push_back(row);
                    currLine++;
                }
                initializeDeadStates();
            }
            else
                cout<<"\nCannot open input file.";
            if (m_boxes.size() != m_goals.size())
            {
                m_invalid = true;
            }
            m_invalid = false;
        }
        ~Field()
        {

        }
        void initializeDeadStates()
        {
            //This method finds corners in the map (can be more than 4).
            //If a box reaches one of these corners then the level becomes unsolvable
            //If a corner is a goal then it is not marked as a dead state
            for (int x=0; x<m_map.size(); x++)
                for (int y=0; y<m_map[x].size();y++)
                    if (m_map[x][y].getType() == EMPTY)
                    {
                        Square down = (m_map)[x+1][y];
                        Square up = (m_map)[x-1][y];
                        Square left = (m_map)[x][y-1];
                        Square right = (m_map)[x][y+1];
                        if ((up.getType() == WALL || down.getType() == WALL)
                                && (left.getType() == WALL || right.getType() == WALL))
                        {
                            m_deadStates.push_back(Point(x,y));
                        }
                    }

            //After finding the corners I am finding all squares between 2 corners which lie along a wall
            std::vector<Point> along_wall;
            bool mark = true;
            for (int i=0; i<m_deadStates.size()-1; i++)
                for (int j=i+1; j<m_deadStates.size(); j++)
                {
                    int x1 = m_deadStates[i].getX();
                    int x2 = m_deadStates[j].getX();
                    int y1 = m_deadStates[i].getY();
                    int y2 = m_deadStates[j].getY();
                    if (x1 == x2)
                    {
                        if ((m_map)[x1-1][y1].getType() == WALL && (m_map)[x2-1][y2].getType() == WALL)
                        {
                            mark = true;
                            int start = (y1<y2) ? y1 : y2;
                            int end = (start == y1) ? y2 : y1;
                            for (int k=start+1; k < end; k++)
                            {
                                if ((m_map)[x1-1][k].getType() != WALL)
                                {
                                    mark = false;
                                    break;
                                }
                                else
                                {
                                    along_wall.push_back(Point(x1,k));
                                }
                            }

                        }
                        if ((m_map)[x1+1][y1].getType() == WALL && (m_map)[x2+1][y2].getType() == WALL)
                        {
                            mark = true;
                            int start = (y1<y2) ? y1 : y2;
                            int end = (start == y1) ? y2 : y1;
                            for (int k=start+1; k < end; k++)
                            {
                                if ((m_map)[x1+1][k].getType() != WALL)
                                {
                                    mark = false;
                                    break;
                                }
                                else
                                {
                                    along_wall.push_back(Point(x1,k));
                                }
                            }
                        }

                    }
                    else if (y1 == y2)
                    {
                        if ((m_map)[x1][y1-1].getType() == WALL && (m_map)[x2][y2-1].getType() == WALL)
                        {
                            mark = true;
                            int start = (x1<x2) ? x1 : x2;
                            int end = (start == x1) ? x2 : x1;
                            for (int k=start+1; k < end; k++)
                            {
                                if ((m_map)[k][y1-1].getType() != WALL)
                                {
                                    mark = false;
                                    break;
                                }
                                else
                                {
                                    along_wall.push_back(Point(k,y1));
                                }
                            }

                        }
                        if ((m_map)[x1][y1+1].getType() == WALL && (m_map)[x2][y2+1].getType() == WALL)
                        {
                            mark = true;
                            int start = (x1<x2) ? x1 : x2;
                            int end = (start == x1) ? x2 : x1;
                            for (int k=start+1; k < end; k++)
                            {
                                if ((m_map)[k][y1+1].getType() != WALL)
                                {
                                    mark = false;
                                    break;
                                }
                                else
                                {
                                    along_wall.push_back(Point(k,y1));
                                }
                            }
                        }
                    }
                }
                if (mark == true)
                            for (int i=0; i<along_wall.size(); i++)
                                m_deadStates.push_back(along_wall[i]);
        }
        vector<Point> getDeadStates()
        const {
            return m_deadStates;
        }
        vector<vector<Square> > getMap()
        const {
            return m_map;
        }
        Point getPlayer()
        {
            return m_player;
        }
        vector<Point> getBoxes()
        {
            return m_boxes;
        }
        vector<Point> getGoals()
        const{
            return m_goals;
        }
        bool isValid()
        {
            return !m_invalid;
        }
    protected:
    private:
        vector<vector<Square> > m_map;
        Point m_player;
        vector<Point> m_boxes;
        vector<Point> m_goals;
        vector<Point> m_deadStates;
        bool m_invalid;
};

#endif FIELD_H

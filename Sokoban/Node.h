#ifndef NODE_H
#define NODE_H
#include <algorithm>
#include <math.h>
using namespace std;
class Node
{
    public:
        Node()
        {
            m_player = Point(-1,-1);
            m_parent = NULL;
            m_path_from_root = "";
            m_visited = false;
            m_ucsCost = 0;
            m_heuristic = 0;
            m_evalFun = 0;
        }
        Node(Point player, vector<Point> boxes, const Node* parent, std::string pathFromRoot)
        {
            m_player = player;
            m_boxes = boxes;
            m_parent = parent;
            m_path_from_root = pathFromRoot;
            m_ucsCost = 0;
            m_heuristic = 0;
            m_visited = false;
            m_evalFun = 0;
        }
        Node(Point player, vector<Point> boxes, const Node* parent, std::string pathFromRoot,
                int ucsCost,vector<Point> goals, int whichHeuristic)
        {
            m_player = player;
            m_boxes = boxes;
            m_parent = parent;
            m_path_from_root = pathFromRoot;
            m_visited = false;
            m_ucsCost = ucsCost;
            calculateHeuristic(goals, whichHeuristic);
            m_evalFun = m_path_from_root.length() + m_heuristic;
            if ((parent != NULL) && (m_evalFun < parent->getEvalFun()))
                m_evalFun = parent->getEvalFun();
        }
        ~Node()
        {
        }
        Point getPlayer()
        const {
            return m_player;
        }
        vector<Point> getBoxes()
        const {
            return m_boxes;
        }
        const Node* getParent()
        const {
            return m_parent;
        }
        bool operator< (const Node& other) const{
            if (!m_player.equals(other.getPlayer()))
                return m_player.less(other.getPlayer());
            else
            {
                for (int i = 0; i < m_boxes.size(); i++)
                {
                    if (!m_boxes[i].equals(other.getBoxes()[i]))
                        return m_boxes[i].less(other.getBoxes()[i]);
                }

            }
            return false;
        }
        bool isGoalState(const Field& field)
        const {
            std::vector<std::vector<Square> > map = field.getMap();
            for (int i=0; i < m_boxes.size(); i++)
            {
                if (map[m_boxes[i].getX()][m_boxes[i].getY()].getType() != GOAL)
                {
                    return false;
                }
            }
            return true;
        }

        bool isDeadlockState(const Field& field)
        const {
            for (int i=0; i<m_boxes.size(); i++)
            {
                if (m_boxes[i].findInVector(field.getDeadStates()) > 0)
                    return true;
            }

            //If no box was in a dead state, then check for frozen boxes:
            //i.e two boxes agains each other and both against a wall
            std::vector<std::vector<Square> > map = field.getMap();
            for (int i=0; i<m_boxes.size(); i++)
            {
                Point up = Point(m_boxes[i].getX() - 1,m_boxes[i].getY());
                Point down = Point(m_boxes[i].getX() + 1,m_boxes[i].getY());
                Point left = Point(m_boxes[i].getX(),m_boxes[i].getY()-1);
                Point right = Point(m_boxes[i].getX() - 1,m_boxes[i].getY()+1);
                if (up.findInVector(m_boxes) > 0)
                {
                    if ((map[m_boxes[i].getX()][m_boxes[i].getY()-1].getType() == WALL && map[m_boxes[i].getX()-1][m_boxes[i].getY()-1].getType() == WALL)
                        || (map[m_boxes[i].getX()][m_boxes[i].getY()+1].getType() == WALL && map[m_boxes[i].getX()-1][m_boxes[i].getY()+1].getType() == WALL))
                    {
                        return true;
                    }
                }
                else if (down.findInVector(m_boxes) > 0)
                {
                    if ((map[m_boxes[i].getX()][m_boxes[i].getY()-1].getType() == WALL && map[m_boxes[i].getX()+1][m_boxes[i].getY()-1].getType() == WALL)
                        || (map[m_boxes[i].getX()][m_boxes[i].getY()+1].getType() == WALL && map[m_boxes[i].getX()+1][m_boxes[i].getY()+1].getType() == WALL))
                     {
                        return true;
                    }
                }
                else if (left.findInVector(m_boxes) > 0)
                {
                    if ((map[m_boxes[i].getX()+1][m_boxes[i].getY()].getType() == WALL && map[m_boxes[i].getX()+1][m_boxes[i].getY()-1].getType() == WALL)
                        || (map[m_boxes[i].getX()-1][m_boxes[i].getY()].getType() == WALL && map[m_boxes[i].getX()-1][m_boxes[i].getY()-1].getType() == WALL))
                     {
                        return true;
                    }
                }
                else if (right.findInVector(m_boxes) > 0)
                {
                    if ((map[m_boxes[i].getX()+1][m_boxes[i].getY()].getType() == WALL && map[m_boxes[i].getX()+1][m_boxes[i].getY()+1].getType() == WALL)
                        || (map[m_boxes[i].getX()-1][m_boxes[i].getY()].getType() == WALL && map[m_boxes[i].getX()-1][m_boxes[i].getY()+1].getType() == WALL))
                    {
                        return true;
                    }
                }
            }
            return false;
        }

        std::vector<Node> expandNode(const Field& field)
        {
            return expandNode(field, 0);
        }
        std::vector<Node> expandNode(const Field& field, int whichHeuristic)
        const {
            std::vector<Node> successors;
            //These coordinates are in a 2-d matrix format not Cartesian coordinates
            //Origin is 0,0 or the first element of the map matrix
            int x = m_player.getX();
            int y = m_player.getY();
            std::vector<std::vector<Square> > map = field.getMap();
            Square down = (map)[x+1][y];
            Square up = (map)[x-1][y];
            Square left = (map)[x][y-1];
            Square right = (map)[x][y+1];
            int indexFound = -1;

            //Check if there isn't a wall on the right of the player
            if (right.getType() == EMPTY || right.getType() == GOAL)
            {
                //Check if right square has a box
                indexFound = right.getPoint().findInVector(m_boxes);
                if (indexFound > -1)
                {
                    Square rightRight = (map)[x][y+2];
                    //Check if the box on the right can be moved to its right
                    //i.e. the square 2 squares right to the player is either empty or a goal and doesn't have another box on it
                    if ((rightRight.getType() == EMPTY || rightRight.getType() == GOAL)
                          && rightRight.getPoint().findInVector(m_boxes) < 0)
                    {
                        Point playerRight = Point(x,y+1);
                        vector<Point> newBoxes(m_boxes);
                        newBoxes[indexFound].setY(newBoxes[indexFound].getY() + 1);
                        std::string newPath(m_path_from_root);
                        newPath.append(1, PUSH_RIGHT);
                        successors.push_back(Node(playerRight,newBoxes,this,newPath,m_ucsCost+2,field.getGoals(), whichHeuristic));
                    }
                }
                //Can move the player to the right
                else
                {
                    Point playerRight = Point(x,y+1);
                    vector<Point> newBoxes(m_boxes);
                    std::string newPath(m_path_from_root);
                    newPath.append(1, MOVE_RIGHT);
                    successors.push_back(Node(playerRight,newBoxes,this,newPath,m_ucsCost+1,field.getGoals(), whichHeuristic));
                }
            }

            //Check if there isn't a wall on the left of the player
            if (left.getType() == EMPTY || left.getType() == GOAL)
            {
                //Check if left square has a box
                indexFound = left.getPoint().findInVector(m_boxes);
                if (indexFound > -1)
                {
                    Square leftLeft = (map)[x][y-2];
                    //Check if the box on the left can be moved to its left
                    //i.e. the square 2 squares left to the player is either empty or a goal and doesn't have another box on it
                    if ((leftLeft.getType() == EMPTY || leftLeft.getType() == GOAL)
                          && leftLeft.getPoint().findInVector(m_boxes) < 0)
                    {
                        Point playerLeft = Point(x,y-1);
                        vector<Point> newBoxes(m_boxes);
                        newBoxes[indexFound].setY(newBoxes[indexFound].getY() - 1);
                        std::string newPath(m_path_from_root);
                        newPath.append(1, PUSH_LEFT);
                        successors.push_back(Node(playerLeft,newBoxes,this,newPath,m_ucsCost+2,field.getGoals(), whichHeuristic));
                    }
                }
                //Can move the player to the left
                else
                {
                    Point playerLeft = Point(x,y-1);
                    vector<Point> newBoxes(m_boxes);
                    std::string newPath(m_path_from_root);
                    newPath.append(1, MOVE_LEFT);
                    successors.push_back(Node(playerLeft,newBoxes,this,newPath,m_ucsCost+1,field.getGoals(), whichHeuristic));
                }
            }

            //Check if there isn't a wall on the top of the player
            if (up.getType() == EMPTY || up.getType() == GOAL)
            {
                //Check if top square has a box
                indexFound = up.getPoint().findInVector(m_boxes);
                if (indexFound > -1)
                {
                    Square upUp = (map)[x-2][y];
                    //Check if the box on the top can be moved to its top
                    //i.e. the square 2 squares left to the player is either empty or a goal and doesn't have another box on it
                    if ((upUp.getType() == EMPTY || upUp.getType() == GOAL)
                          && upUp.getPoint().findInVector(m_boxes) < 0)
                    {
                        Point playerUp = Point(x-1,y);
                        vector<Point> newBoxes(m_boxes);
                        newBoxes[indexFound].setX(newBoxes[indexFound].getX() - 1);
                        std::string newPath(m_path_from_root);
                        newPath.append(1, PUSH_UP);
                        successors.push_back(Node(playerUp,newBoxes,this,newPath,m_ucsCost+2,field.getGoals(), whichHeuristic));
                    }
                }
                //Can move the player up
                else
                {
                    Point playerUp = Point(x-1,y);
                    vector<Point> newBoxes(m_boxes);
                    std::string newPath(m_path_from_root);
                    newPath.append(1, MOVE_UP);
                    successors.push_back(Node(playerUp,newBoxes,this,newPath,m_ucsCost+1,field.getGoals(), whichHeuristic));
                }
            }

            //Check if there isn't a wall below the player
            if (down.getType() == EMPTY || down.getType() == GOAL)
            {
                //Check if bottom square has a box
                indexFound = down.getPoint().findInVector(m_boxes);
                if (indexFound > -1)
                {
                    Square downDown = (map)[x+2][y];
                    //Check if the box below can be moved further below
                    //i.e. the square 2 squares below the player is either empty or a goal and doesn't have another box on it
                    if ((downDown.getType() == EMPTY || downDown.getType() == GOAL)
                          && downDown.getPoint().findInVector(m_boxes) < 0)
                    {
                        Point playerDown = Point(x+1,y);
                        vector<Point> newBoxes(m_boxes);
                        newBoxes[indexFound].setX(newBoxes[indexFound].getX() + 1);
                        std::string newPath(m_path_from_root);
                        newPath.append(1, PUSH_DOWN);
                        successors.push_back(Node(playerDown,newBoxes,this,newPath,m_ucsCost+2,field.getGoals(), whichHeuristic));
                    }
                }
                //Can move the player down
                else
                {
                    Point playerDown = Point(x+1,y);
                    vector<Point> newBoxes(m_boxes);
                    std::string newPath(m_path_from_root);
                    newPath.append(1, MOVE_DOWN);
                    successors.push_back(Node(playerDown,newBoxes,this,newPath,m_ucsCost+1,field.getGoals(), whichHeuristic));
                }
            }
            return successors;
        }

        std::string getPathFromRoot()
        {
            return m_path_from_root;
        }
        bool isVisited()
        {
            return m_visited;
        }
        void setVisited(bool visited)
        {
            m_visited = visited;
        }
        int getUCSCost()
        const{
            return m_ucsCost;
        }
        void setVisited(int ucsCost)
        {
            m_ucsCost = ucsCost;
        }
        int getHeuristic()
        const{
            return m_heuristic;
        }
        int getEvalFun()
        const{
            return m_evalFun;
        }

    protected:
    private:
        void calculateHeuristic(vector<Point> goals, int whichHeuristic)
        {
            int heuristic = 0;
            switch(whichHeuristic)
            {
                case 0:
                    //0 means the node will be used for BFS/DFS/UCS and a heuristic doesn't need to be calculated
                    break;
                case 1:
                    //1 means calculate the first heuristic, i.e., the sum of Manhattan distances of each box from its nearest goal
                    {
                        int min_manhattan_sum = 0;
                        for (int i=0; i < m_boxes.size(); i++)
                        {
                            int min_i = abs(m_boxes[i].getX() - goals[0].getX())
                                                + abs(m_boxes[i].getY() - goals[0].getY());
                            for (int j=1; j < goals.size(); j++)
                            {
                                int dist_j = abs(m_boxes[i].getX() - goals[j].getX())
                                                + abs(m_boxes[i].getY() - goals[j].getY());
                                if (dist_j < min_i)
                                    min_i = dist_j;
                            }
                            min_manhattan_sum += min_i;
                        }
                        heuristic = (int)min_manhattan_sum;
                    }
                    break;
                case 2:
                    //2 means calculate the second heuristic, i.e.,sum of Euclidian distance of each box from the nearest goal
                    {
                        float min_euclidian_sum = 0.0;
                        for (int i=0; i < m_boxes.size(); i++)
                        {
                            float min_i = sqrt((float)(square(m_boxes[i].getX() - goals[0].getX())
                                                + square(m_boxes[i].getY() - goals[0].getY())));
                            for (int j=1; j < goals.size(); j++)
                            {
                                float dist_j = sqrt((float)square(m_boxes[i].getX() - goals[j].getX())
                                                + square(m_boxes[i].getY() - goals[j].getY()));
                                if (dist_j < min_i)
                                    min_i = dist_j;
                            }
                            min_euclidian_sum += min_i;
                        }
                        heuristic = min_euclidian_sum;
                    }
                    break;
                default:
                    break;
            }
            m_heuristic = heuristic;
        }
        int square (int x)
        {
            return x*x;
        }
        Point m_player;
        vector<Point> m_boxes;
        const Node* m_parent;
        bool m_visited;
        //List of player moves which take the game from the initial state
        //to the state represented by this node
        std::string m_path_from_root;
        int m_ucsCost;
        unsigned int m_heuristic;
        unsigned int m_evalFun;
};
class UCSNodeComparator{
public:
    bool operator() (const Node& n1, const Node& n2) const{
        if (n1.getUCSCost() < n2.getUCSCost())
            return true;
        else if (n1.getUCSCost() > n2.getUCSCost())
            return false;
        if (!n1.getPlayer().equals(n2.getPlayer()))
                return n1.getPlayer().less(n2.getPlayer());
            else
            {
                for (int i = 0; i < n1.getBoxes().size(); i++)
                {
                    if (!n1.getBoxes()[i].equals(n2.getBoxes()[i]))
                        return n1.getBoxes()[i].less(n2.getBoxes()[i]);
                }

            }
            return false;
    }
};

#endif // NODE_H

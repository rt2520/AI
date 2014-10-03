#ifndef SEARCH_H
#define SEARCH_H
#include <queue>
#include <stack>
#include "Node.h"
#include "Field.h"

using namespace std;

class Search
{
    public:
        Search()
        {
            m_nodesGen = 0;
            m_repeatedNodesGen = 0;
            m_fringeLength = 0;
            m_exploredListLength = 0;
            m_deadlockStatesGen = 0;
        }
        ~Search()
        {
        }
        unsigned long getNodesGen()
        {
            return m_nodesGen;
        }
        unsigned long getRepeatedNodesGen()
        {
            return m_repeatedNodesGen;
        }
        unsigned long getFringeLength()
        {
            return m_fringeLength;
        }
        unsigned long getExploredListLength()
        {
            return m_exploredListLength;
        }
        unsigned long getDeadlocksGen()
        {
            return m_deadlockStatesGen;
        }

        Node performBFS(Field field, Node root, bool checkDeads)
        {
            m_nodesGen = 1;
            std::queue<Node> fringe;
            std::set<Node> exploredList;
            fringe.push(root);
            m_fringeLength = 1;
            while (!fringe.empty() && m_nodesGen < 30000)
            {
                Node parent = fringe.front();
                fringe.pop();
                m_fringeLength--;
                if (findInExplored(exploredList,parent))
                    m_repeatedNodesGen++;
                else
                {
                    exploredList.insert(parent);
                    m_exploredListLength = exploredList.size();
                    if (parent.isGoalState(field))
                        return parent;
                    if (checkDeads && parent.isDeadlockState(field))
                        m_deadlockStatesGen++;
                    else
                    {
                        std::vector<Node> successors = parent.expandNode(field);
                        for (int i=0; i < successors.size(); i++)
                        {
                            m_nodesGen++;
                            if (!findInExplored(exploredList,successors[i]))
                            {
                                fringe.push(successors[i]);
                            }
                            else
                            {
                                m_repeatedNodesGen++;
                            }

                        }
                        m_fringeLength = fringe.size();
                    }
                }
            }
            vector<Point> empty;
            return Node(Point(-1,-1),empty,NULL,"");
        }

        Node performDFS(Field field, Node root, bool checkDeads)
        {
            m_nodesGen = 1;
            std::stack<Node> fringe;
            std::set<Node> exploredList;
            fringe.push(root);
            m_fringeLength = 1;
            while (!fringe.empty() && m_nodesGen < 30000)
            {
                Node parent = fringe.top();
                fringe.pop();
                m_fringeLength--;
                if (findInExplored(exploredList,parent))
                    m_repeatedNodesGen++;
                else
                {

                    exploredList.insert(parent);
                    m_exploredListLength = exploredList.size();
                    if (parent.isGoalState(field))
                        return parent;
                    if (checkDeads && parent.isDeadlockState(field))
                        m_deadlockStatesGen++;
                    else if (parent.getPathFromRoot().length() <= 150)
                    {
                        std::vector<Node> successors = parent.expandNode(field);
                        for (int i=successors.size() - 1; i >= 0; i--)
                        {
                            m_nodesGen++;
                            if (!findInExplored(exploredList,successors[i]))
                            {
                                fringe.push(successors[i]);
                            }
                            else
                            {
                                m_repeatedNodesGen++;
                            }

                        }
                        m_fringeLength = fringe.size();
                    }
                }
            }
            vector<Point> empty;
            return Node(Point(-1,-1),empty,NULL,"");
        }
        Node performUCS(Field field, Node root, bool checkDeads)
        {
            m_nodesGen = 1;
            std::vector<Node> fringe;
            std::set<Node> exploredList;
            fringe.push_back(root);
            m_fringeLength = 1;
            int min = 0;
            while (!fringe.empty() && m_nodesGen < 30000)
            {
                min = 0;
                for (int i=1; i<fringe.size(); i++)
                {
                    if (fringe[i].getUCSCost() < fringe[min].getUCSCost())
                        min = i;
                }
                Node parent = fringe[min];
                fringe.erase(fringe.begin() + min);
                m_fringeLength--;
                if (findInExplored(exploredList,parent))
                    m_repeatedNodesGen++;
                else
                {
                    exploredList.insert(parent);
                    m_exploredListLength = exploredList.size();
                    if (parent.isGoalState(field))
                    {
                        return parent;
                    }
                    if (checkDeads && parent.isDeadlockState(field))
                    {
                        m_deadlockStatesGen++;
                    }
                    else
                    {
                        std::vector<Node> successors = parent.expandNode(field);
                        for (int i=0; i < successors.size(); i++)
                        {
                            m_nodesGen++;
                            if (!findInExplored(exploredList,successors[i]))
                            {
                                fringe.push_back(successors[i]);
                            }
                            else
                            {
                                m_repeatedNodesGen++;
                            }

                        }
                        m_fringeLength = fringe.size();
                    }
                }
            }
            vector<Point> empty;
            return Node(Point(-1,-1),empty,NULL,"");
        }
        Node performGreedySearch(Field field, Node root, int whichHeuristic, bool checkDeads)
        {
            m_nodesGen = 1;
            std::vector<Node> fringe;
            std::set<Node> exploredList;
            fringe.push_back(root);
            m_fringeLength = 1;
            int min = 0;
            while (!fringe.empty() && m_nodesGen < 30000)
            {
                min = 0;
                for (int i=0; i<fringe.size(); i++)
                {
                    if (fringe[i].getHeuristic() < fringe[min].getHeuristic())
                        min = i;
                }
                Node parent = fringe[min];
                fringe.erase(fringe.begin() + min);
                m_fringeLength--;
                if (findInExplored(exploredList,parent))
                    m_repeatedNodesGen++;
                else
                {
                    exploredList.insert(parent);
                    m_exploredListLength = exploredList.size();
                    if (parent.isGoalState(field))
                    {
                        return parent;
                    }
                    if (checkDeads && parent.isDeadlockState(field))
                        m_deadlockStatesGen++;
                    else
                    {
                        std::vector<Node> successors = parent.expandNode(field, whichHeuristic);
                        for (int i=0; i < successors.size(); i++)
                        {
                            m_nodesGen++;
                            if (!findInExplored(exploredList,successors[i]))
                            {
                                fringe.push_back(successors[i]);
                            }
                            else
                            {
                                m_repeatedNodesGen++;
                            }

                        }
                        m_fringeLength = fringe.size();
                    }
                }
            }
            vector<Point> empty;
            return Node(Point(-1,-1),empty,NULL,"");
        }

        Node performAStarSearch(Field field, Node root, int whichHeuristic, bool checkDeads)
        {
            m_nodesGen = 1;
            std::vector<Node> fringe;
            std::set<Node> exploredList;
            fringe.push_back(root);
            m_fringeLength = 1;
            int min = 0;
            while (!fringe.empty() && m_nodesGen < 30000)
            {
                min = 0;
                for (int i=0; i<fringe.size(); i++)
                {
                    if (fringe[i].getEvalFun() < fringe[min].getEvalFun())
                        min = i;
                }
                Node parent = fringe[min];
                fringe.erase(fringe.begin() + min);
                m_fringeLength--;
                if (findInExplored(exploredList,parent))
                    m_repeatedNodesGen++;
                else
                {
                    exploredList.insert(parent);
                    m_exploredListLength = exploredList.size();
                    if (parent.isGoalState(field))
                    {
                        return parent;
                    }
                    if (checkDeads && parent.isDeadlockState(field))
                        m_deadlockStatesGen++;
                    else
                    {
                        std::vector<Node> successors = parent.expandNode(field, whichHeuristic);
                        for (int i=0; i < successors.size(); i++)
                        {
                            m_nodesGen++;
                            if (!findInExplored(exploredList,successors[i]))
                            {
                                fringe.push_back(successors[i]);
                            }
                            else
                            {
                                m_repeatedNodesGen++;
                            }

                        }
                        m_fringeLength = fringe.size();
                    }
                }
            }
            vector<Point> empty;
            return Node(Point(-1,-1),empty,NULL,"");
        }
        bool findInExplored(std::set<Node> exploredList, Node parent)
        {
            if (exploredList.find(parent) != exploredList.end())
                return true;
            int found = 0;
            vector<Point> boxes = parent.getBoxes();
            for (set<Node>::iterator it = exploredList.begin(); it!=exploredList.end(); it++)
            {
                if (parent.getPlayer().equals(it->getPlayer()))
                {
                    found = 0;
                    for (int i=0;i<boxes.size();i++)
                    {
                        if (boxes[i].findInVector(it->getBoxes()) > -1)
                            found++;
                        else
                            break;
                    }
                    if (found == boxes.size())
                        return true;
                }
            }
            return false;
        }
    protected:
    private:
        unsigned long m_nodesGen;
        unsigned long m_repeatedNodesGen;
        unsigned long m_fringeLength;
        unsigned long m_exploredListLength;
        unsigned long m_deadlockStatesGen;
};

#endif // Search_H

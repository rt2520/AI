#include <iostream>
#include <vector>
#include <set>
#include <ctime>
const int EMPTY = 0;
const int WALL = 1;
const int GOAL = 2;
const char MOVE_RIGHT = 'r';
const char MOVE_LEFT = 'l';
const char MOVE_UP = 'u';
const char MOVE_DOWN = 'd';
const char PUSH_RIGHT = 'r';
const char PUSH_LEFT = 'l';
const char PUSH_UP = 'u';
const char PUSH_DOWN = 'd';

#include "Field.h"
#include "Square.h"
#include "Node.h"
#include "Search.h"

using namespace std;

void showStats(Node result, Search bfsObj)
{
    if (result.getPlayer().getX() > -1)
    {
        std::string moves = result.getPathFromRoot();
        cout<<endl<<"Solution: ";
        int i=0;
        for (;i < moves.size() - 1; i++)
            cout<<moves.at(i)<<",";
        cout<<moves.at(i);
        cout<<endl<<"No of moves: "<<result.getPathFromRoot().size();
    }
    else
    {
        cout<<endl<<"NO SOLUTION FOUND!!!";
    }
    char showStat = 'n';
    cout<<endl<<"Do you want to see the statistics of the search? [y/n]: ";
    cin>>showStat;
    if (showStat == 'y' || showStat == 'y')
    {
        cout<<endl<<"STATISTICS:\n";
        cout<<endl<<"Total states generated: "<<bfsObj.getNodesGen();
        cout<<endl<<"States explored: "<<bfsObj.getExploredListLength();
        cout<<endl<<"Fringe length on termination: "<<bfsObj.getFringeLength();
        cout<<endl<<"Repeated states: "<<bfsObj.getRepeatedNodesGen();
        cout<<endl<<"Deadlock states: "<<bfsObj.getDeadlocksGen();
    }
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        cout<<endl<<"Please input the file name with the pattern!!! Exiting!!!"<<endl;
        return 1;
    }

    char* inputFile = argv[1];
    Field field = Field(inputFile);
    if (!field.isValid())
    {
        cout<<endl<<"NO OF BOXES DOES NOT MATCH NO OF GOALS!!! ABORTING!!!";
        return -1;
    }
    int search = -1;
    while (search == -1)
    {
        cout<<"Please enter a number between 0 and 5 to choose from the following:";
        cout<<endl<<"0. Exit"<<endl<<"1. Breadth First Search";
        cout<<endl<<"2. Depth First Search"<<endl<<"3. Uniform Cost Search";
        cout<<endl<<"4. Best First Search (Greedy)"<<endl<<"5. A* search"<<endl<<"Your Choice: ";
        cin>>search;
        Search bfsObj;
        char checkDeadlocks = 'n';
        bool checkDeads = false;
        Node result;
        Node root;
        int whichHeuristic = 0;
        switch(search)
        {
            case 0:
                cout<<endl<<"Exiting!!"<<endl;
                return 0;
            case 1:
            {
                cout<<endl<<"You chose Breadth First Search: ";
                cout<<endl<<"Do you want the algorithm to detect deadlocks? [y/n]:";
                cin>>checkDeadlocks;
                checkDeads = (checkDeadlocks == 'y' || checkDeadlocks == 'Y') ? true : false;
                root = Node(field.getPlayer(), field.getBoxes(), NULL, "", 0, field.getGoals(), whichHeuristic);
                clock_t t = clock();
                result = bfsObj.performBFS(field,root,checkDeads);
                cout<<endl<<"Time taken: "<<(double)(((double)(clock()-t))/CLOCKS_PER_SEC)<<" seconds.";
                showStats(result,bfsObj);
                //search = -1;
                break;
            }
            case 2:
            {
                cout<<endl<<"You chose Depth First Search: ";
                cout<<endl<<"Do you want the algorithm to detect deadlocks? [y/n]:";
                cin>>checkDeadlocks;
                checkDeads = (checkDeadlocks == 'y' || checkDeadlocks == 'Y') ? true : false;
                root = Node(field.getPlayer(), field.getBoxes(), NULL, "", 0, field.getGoals(), whichHeuristic);
                clock_t t = clock();
                result = bfsObj.performDFS(field,root,checkDeads);
                cout<<endl<<"Time taken: "<<(double)(((double)(clock()-t))/CLOCKS_PER_SEC)<<" seconds.";
                showStats(result,bfsObj);
                //search = -1;
                break;
            }
            case 3:
            {
                cout<<endl<<"You chose Uniform Cost Search: ";
                cout<<endl<<"Do you want the algorithm to detect deadlocks? [y/n]:";
                cin>>checkDeadlocks;
                checkDeads = (checkDeadlocks == 'y' || checkDeadlocks == 'Y') ? true : false;
                root = Node(field.getPlayer(), field.getBoxes(), NULL, "", 0, field.getGoals(), whichHeuristic);
                clock_t t = clock();
                result = bfsObj.performUCS(field,root,checkDeads);
                cout<<endl<<"Time taken: "<<(double)(((double)(clock()-t))/CLOCKS_PER_SEC)<<" seconds.";
                showStats(result,bfsObj);
                //search = -1;
                break;
            }
            case 4:
            {
                cout<<endl<<"You chose Best First Search(Greedy): ";
                cout<<endl<<"Please select a heuristic: ";
                cout<<endl<<"1. Manhattan distance\n2. Euclidian distance";
                while (whichHeuristic != 1 && whichHeuristic != 2)
                {
                    cout<<endl<<"Your choice [1/2]:";
                    cin>>whichHeuristic;
                }
                cout<<endl<<"Do you want the algorithm to detect deadlocks? [y/n]:";
                cin>>checkDeadlocks;
                checkDeads = (checkDeadlocks == 'y' || checkDeadlocks == 'Y') ? true : false;
                root = Node(field.getPlayer(), field.getBoxes(), NULL, "", 0, field.getGoals(), whichHeuristic);
                clock_t t = clock();
                result = bfsObj.performGreedySearch(field,root,whichHeuristic,checkDeads);
                cout<<endl<<"Time taken: "<<(double)(((double)(clock()-t))/CLOCKS_PER_SEC)<<" seconds.";
                showStats(result,bfsObj);
                //search = -1;
                break;
            }
            case 5:
            {
                cout<<endl<<"You chose A* Search: ";
                cout<<endl<<"Please select a heuristic: ";
                cout<<endl<<"1. Manhattan distance\n2. Euclidian distance";
                while (whichHeuristic != 1 && whichHeuristic != 2)
                {
                    cout<<endl<<"Your choice [1/2]:";
                    cin>>whichHeuristic;
                }
                cout<<endl<<"Do you want the algorithm to detect deadlocks? [y/n]:";
                cin>>checkDeadlocks;
                checkDeads = (checkDeadlocks == 'y' || checkDeadlocks == 'Y') ? true : false;
                root = Node(field.getPlayer(), field.getBoxes(), NULL, "", 0, field.getGoals(), whichHeuristic);
                clock_t t = clock();
                result = bfsObj.performAStarSearch(field,root,whichHeuristic,checkDeads);
                cout<<endl<<"Time taken: "<<(double)(((double)(clock()-t))/CLOCKS_PER_SEC)<<" seconds.";
                showStats(result,bfsObj);
                //search = -1;
                break;
            }
            default:
                break;
        }
        char cont = 'n';
        cout<<endl<<"Do you want to continue? [y/n]";
        cin>>cont;
        if (cont == 'y' || cont == 'Y')
            search = -1;
        else
            search = 0;
    }
    return 0;
}

/*Name: Rahul Tewari
UNI: rt2520*/
#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <functional>
#include <set>
#include <string>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <limits.h>
#include <ctime>
#include <cstdlib>

using namespace std;

char DOT = 'o';
char CROSS = 'x';
const char EMPTY = '.';
int n = 15;
int m = 5;
clock_t t;
int s = 60;
int mode = 0;

class Move {
    public:
        Move()
        {
            m_x = -1;
            m_y = -1;
            m_type = EMPTY;
        }
        Move(int x, int y, char type)
        {
            m_x = x;
            m_y = y;
            m_type = type;
        }
        ~Move()
        {

        }
        int getX()
        const {
            return m_x;
        }
        int getY()
        const {
            return m_y;
        }
        void setX(int x)
        {
            m_x = x;
        }
        void setY(int y)
        {
            m_y = y;
        }
        void setType(char t)
        {
            m_type = t;
        }
        char getType()
        {
            return m_type;
        }
        bool equals(const Move& a)
        const {
            return (m_x == a.getX() && m_y == a.getY());
        }
        bool less(const Move& a)
        const {
            if (this->equals(a))
                return false;
            int a_x = a.getX();
            int a_y = a.getY();
            if (m_x != a_x)
                return (m_x < a_x);
            return (m_y < a_y) ;
        }
        int findInVector(const std::vector<Move>& list)
        const {
            for (int i = 0; i < list.size(); i++)
                if (equals(list[i]))
                    return i;
            return -1;
        }
    private:
        int m_x;
        int m_y;
        char m_type;

};


//Class representing the state as a node
class Board
{
    public:
        Board(vector<char> config, int x, int y, char turn, int value, bool isTerminal)
        {
            m_config = config;
            m_value = value;
            m_move = Move(x,y,turn);
            m_isTerminal = isTerminal;
        }
        ~Board() {}
        bool operator < (const Board& b) const
        {
            return (m_value < b.getValue());
        }
        bool isTerminal()
        {
            return (m_isTerminal);// || m_value == INT_MIN + 1 || m_value == INT_MAX);
        }
        void setTerminal(bool term)
        {
            m_isTerminal = term;
        }
        vector<char> getConfig()
        const {
            return m_config;
        }
        int getValue()
        const{
            return m_value;
        }
        void setValue(int val)
        {
            m_value = val;
        }
        Move getMove()
        const {
            return m_move;
        }
        void setMove(Move p)
        {
            m_move = p;
        }
        int getChainValue(char* slice, int size)
        {
            int xstart = 0, ostart = 0;
            int xlen = 0, olen = 0;
            int curstart = 0;
            int curlen = 0;
            char checkingFor = EMPTY;
            int eval = 0;
            /*cout<<size<<"--";
            for (int j=0; j < size; j++)
                cout<<slice[j];*/
            for (int j=0; j < size; j++)
            {
                if (slice[j] == EMPTY)
                {
                    if (checkingFor == CROSS && curlen > xlen)
                    {
                        xstart = curstart;
                        xlen = curlen;
                    }
                    else if (checkingFor == DOT && curlen > olen)
                    {
                        ostart = curstart;
                        olen = curlen;
                    }
                    curstart = j;
                    curlen = 0;
                }
                else if (slice[j] == CROSS)
                {
                    if (checkingFor == CROSS)
                    {
                        curlen++;
                    }
                    else
                    {
                        if (checkingFor == DOT && curlen > olen)
                        {
                            ostart = curstart;
                            olen = curlen;
                        }
                        curstart = j;
                        curlen = 1;
                    }
                }
                else
                {
                    if (checkingFor == DOT)
                    {
                        curlen++;
                    }
                    else
                    {
                        if (checkingFor == CROSS && curlen > xlen)
                        {
                            xstart = curstart;
                            xlen = curlen;
                        }
                        curstart = j;
                        curlen = 1;
                    }
                }
                checkingFor = slice[j];
            }
            if (checkingFor == CROSS && curlen > xlen)
            {
                xstart = curstart;
                xlen = curlen;
            }
            else if (checkingFor == DOT && curlen > olen)
            {
                ostart = curstart;
                olen = curlen;
            }
            for (int i=0; i < 2; i++)
            {
                int len = (i==0) ? xlen : olen;
                int start = (i==0) ? xstart : ostart;
                char symbol = (i==0) ? CROSS : DOT;
                char other = (symbol == CROSS) ? DOT : CROSS;
                int sign = (i==0) ? 1 : -1;
                if (len == m)
                {
                    return sign*INT_MAX;
                }
                else if (len == m-1 && (start > 0 && slice[start-1] == EMPTY) && (start+len < size && slice[start+len] == EMPTY))
                {
                    //live 4
                    eval += sign*1000000;
                }
                else if (len == m-1 && ((start > 0 && slice[start-1] == EMPTY) || (start+len < size && slice[start+len] == EMPTY)))
                {
                    //sleep 4
                    eval += sign*100000;
                }
                else if (m > 3 && len == m-2)
                {
                    if ((start > 1 && slice[start-1] == EMPTY && slice[start-2] == EMPTY)
                          && (start+len < size-1 && slice[start+len] == EMPTY && slice[start+len+1] == EMPTY))
                          //live 3
                        eval += sign*90000;
                    else if ((start > 1 && slice[start-1] == EMPTY && slice[start-2] == EMPTY)
                             || (start > 0 && slice[start-1] == EMPTY && start+len < size && slice[start+len] == EMPTY)
                               || (start+len < size-1 && slice[start+len] == EMPTY && slice[start+len+1] == EMPTY))
                        //sleep 3
                        eval += sign*2000;
                }
                else if (m > 4 && len == m-3)
                {
                    if (start > 0 && slice[start-1] == EMPTY)
                    {
                        if ((start > 2 && slice[start - 3] == EMPTY && slice[start-2] == symbol && start+len < size && slice[start+len] == EMPTY)
                             || (start+len < size-2 && slice[start+len] == EMPTY && slice[start+len+1] == symbol &&slice[start+len+2] == EMPTY))
                            //non-cons 3
                            eval += 100;
                    }
                }
            }
            return eval;
        }
        bool eval(bool useTT);
        vector<Board> getSuccessors(bool useTT);
        bool operator==(const Board& b)
        const{
            vector<char> config2 = b.getConfig();
            for (int i = 0; i < n; i++)
                for (int j = 0; j < n; j++)
                    if (m_config[n*i + j] != config2[n*i + j])
                        return false;
            return true;
        }
    protected:
    private:
        //Not using char** to avoid writing the copy constructor
        vector<char> m_config;
        int m_value;
        Move m_move;
        //vector<int> avMoves;
        bool m_isTerminal;
};


struct myHash{
    size_t operator()(const Board& b)
    const{
        int sum = 0;
        vector<char> config = b.getConfig();
            for (int i = 0; i < n; i++)
                for (int j = 0; j < n; j++)
                    if (config[n*i + j] == CROSS)
                        sum += (n*i + j)*(n*i + j)*19 + 47;
                    else if (config[n*i + j] == DOT)
                        sum += (n*i + j)*(n*i + j)*29 + 59;
        return std::hash<int>()(sum);
    }
};
std::unordered_map<Board, int, myHash> transTable;

vector<Board> Board::getSuccessors(bool useTT)
{
    vector<Board> succ;
    char turn = m_move.getType() == CROSS ? DOT : CROSS;
    char* slice = new char[n];
    int eval = 0;
    int empties = 0;
    for (int i=0; i < n; i++)
    {
        for (int j=0; j < n; j++)
        {
            if (m_config[n*i+j] == EMPTY)
            {
                empties++;
                bool isTerminal = false;
                vector<char> config = vector<char>(m_config);
                config[n*i+j] = turn;
                int start = (j-4 > 0) ? j-4 : 0;
                int end = (j+4 < n-1) ? j+4 : n-1;
                int c = 0;
                for (int k = start; k <= end; k++)
                    slice[c++] = config[n*i+k];
                int cv = getChainValue(slice, c);
                if (cv == INT_MIN + 1 || cv == INT_MAX)
                {
                    isTerminal = true;
                    eval = cv;
                }

                if (!isTerminal)
                {
                    start = (i-4 > 0) ? i-4 : 0;
                    end = (i+4 < n-1) ? i+4 : n-1;
                    c = 0;
                    for (int k = start; k <= end; k++)
                        slice[c++] = config[n*k+j];
                    cv = getChainValue(slice, c);
                    if (cv == INT_MIN + 1 || cv == INT_MAX)
                    {
                        isTerminal = true;
                        eval = cv;
                    }
                }

                if (!isTerminal)
                {
                    start = (i-4 > 0) ? i-4 : 0;
                    end = (i+4 < n-1) ? i+4 : n-1;
                    int start2 = (j-4 > 0) ? j-4 : 0;
                    int end2 = (j+4 < n-1) ? j+4 : n-1;
                    int startx = (i - start < j - start2) ? start : i - (j - start2);
                    int starty = (i - start < j - start2) ? j - (i - start) : start2;
                    int endx = (end - i < end2 - j) ? end : i + (end2 - j);
                    int endy = (end - i < end2 - j) ? j + (end - i) : end2;
                    c = 0;
                    int k = startx;
                    int m = starty;
                    for (; k <= endx && m <= endy ; k++, m++)
                    {
                        slice[c++] = config[n*k+m];
                    }
                    cv = getChainValue(slice, c);
                    if (cv == INT_MIN + 1 || cv == INT_MAX)
                    {
                        isTerminal = true;
                        eval = cv;
                    }
                }

                if (!isTerminal)
                {
                    start = (i-4 > 0) ? i-4 : 0;
                    end = (i+4 < n-1) ? i+4 : n-1;
                    int start2 = (j-4 > 0) ? j-4 : 0;
                    int end2 = (j+4 < n-1) ? j+4 : n-1;
                    int startx = (i - start < end2 - j) ? start : i - (end2 - j);
                    int starty = (i - start < end2 - j) ? j + (i - start) : end2;
                    int endx = (end - i < j - start2) ? end : i + (j - start2);
                    int endy = (end - i < j - start2) ? j - (end - i) : start2;
                    c = 0;
                    int k = startx;
                    int m = starty;
                    for (; k <= endx && m <= endy ; k++, m++)
                    {
                        slice[c++] = config[n*k+m];
                    }
                    cv = getChainValue(slice, c);
                    if (cv == INT_MIN + 1 || cv == INT_MAX)
                    {
                        isTerminal = true;
                        eval = cv;
                    }
                }
                Board child(config, i, j, turn, eval, isTerminal);
                if (useTT)
                {
                    std::unordered_map<Board, int, myHash>::iterator res = transTable.find(child);
                    if (res != transTable.end())
                    {
                        child.setValue(res->second);
                    }
                    else
                        child.eval(useTT);
                }
                succ.push_back(child);
            }
        }
    }
    if (succ.size() > 0)
    {
        if (succ.size() == 1)
            succ[0].setTerminal(true);
        else if (useTT)
        {
            sort(succ.begin(), succ.end());
        }
    }
    else
        this->m_isTerminal = true;
    delete[] slice;
    return succ;
}

//EVALUATION FUNCTION
bool Board::eval(bool useTT)
{
    char* slice = new char[n];
    int eval = 0;
    bool done = false;
    bool empty = false;
    for (int i=0; i < n; i++)
    {
        for (int j=0; j < n; j++)
        {
            slice[j] = m_config[n*i+j];
            if (m_config[n*i+j] == EMPTY)
                empty = true;
        }
        //slice.insert(slice.begin(), config[i].begin(), config[i].end());
        int cv = getChainValue(slice, n);
        if (INT_MIN + 1 < cv && cv < INT_MAX)
            eval += cv;
        else
        {
            eval = cv;
            done = true;
            break;
        }
    }
    //slice.erase();
    if (!done)
        for (int j=0; j < n; j++)
        {
            for (int i=0; i < n; i++)
                slice[i] = m_config[n*i+j];
                //slice.push_back(config[n*i+j]);
            int cv = getChainValue(slice, n);
            if (INT_MIN + 1 < cv && cv < INT_MAX)
                eval += cv;
            else
            {
                eval = cv;
                done = true;
                break;
            }
        }
    //slice.erase();
    if (!done)
        for (int d=0; d <= 2*n-1; d++)
        {
            int j = (d < n) ? 0 : d-n+1;
            int ctr = 0;
            for (int k = j; k <= d-j; k++)
            {
                slice[ctr++] = m_config[n*k + d-k];
            }
                //slice.push_back(config[k][d-k]);
            int cv = getChainValue(slice, ctr);
            if (INT_MIN + 1 < cv && cv < INT_MAX)
                eval += cv;
            else
            {
                    eval = cv;
                    done = true;
                    break;
            }
        }
    //slice.erase();
    if (!done)
        for (int d=0; d <= 2*n-1; d++)
        {
            int j = (d < n) ? 0 : d-n+1;
            int ctr = 0;
            for (int k = j; k <= d-j; k++)
            {
                slice[ctr++] = m_config[n*k + n - 1 - d + k];
            }
                //slice.push_back(config[k][d-k]);
            int cv = getChainValue(slice, ctr);
            if (INT_MIN + 1 < cv && cv < INT_MAX)
                eval += cv;
            else
            {
                eval = cv;
                done = true;
                break;
            }
        }
    delete[] slice;
    m_value = eval;
    if (useTT)
    {
        std::pair<Board, int> hashEntry(*this, this->m_value);
        transTable.insert(hashEntry);
    }
    return empty;
//Put this board in TT
}

Move getBestMove(Board& board, int rootVal, bool useTT, short maxDepth);
double getTimeRemaining()
{
    if (mode == 1)
        return ((double)s - (double)((((double)(clock()-t))/CLOCKS_PER_SEC)));
    return 100.00;
}

void print(vector<char> config, int first)
{
    for (int i=0; i < n; i++)
    {
        cout<<endl;
        for (int j=0; j < n; j++)
        {
            char c = config[n*i+j];
            if (c != EMPTY)
                c = (first == 2) ? c : ((c == DOT) ? CROSS : DOT);
            cout<<"|"<<c;
            if ((j+1)%n == 0)
                cout<<"|";
        }
    }
    cout<<endl;
}

int main(int argc, char* argv[])
{
    vector<char> config(n*n, EMPTY);
    char wannaPlay = 'y';
    int first = 0;
    int x,y;
    while (true)
    {
        cout<<"Please enter the game parameters:\nBoard Size(n): ";
        cin>>n;
        cout<<"\nWinning chain length(m): ";
        cin>>m;
        cout<<"\nTime-limit for a move(s): ";
        cin>>s;
        cout<<"Please select the mode of play:"<<endl;
        cout<<"1. Me vs You\n2. Me vs Random Agent\n3. ME vs ME!!"<<endl<<"Your choice: ";
        cin>>mode;
        if (mode == 1 || mode == 2 || mode == 3)
            break;
        else
            cout<<"\nInvalid choice!!"<<endl;

    }
    if (mode == 1)
    {
        while (true)
        {
            cout<<"Please select who goes first:"<<endl;
            cout<<"1. You\n2. Me"<<endl<<"Your choice: ";
            cin>>first;
            if (first == 1 || first == 2)
                break;
            else
                cout<<"\nInvalid choice!!"<<endl;

        }
        if (first == 2)
        {
            Board board(config, -1, -1, DOT, 0, false);
            int mx = n/2;
            int my = n/2;
            config[n*mx + my] = CROSS;
            cout<<"My Move:"<<mx<<" , "<<my<<endl;
            print(config, first);
        }
        while (wannaPlay == 'y' || wannaPlay == 'Y')
        {
            cout<<"\nEnter move:"<<endl;
            cout<<"Row: ";
            cin>>x;
            cout<<"Column: ";
            cin>>y;
            t = clock();
            if (x >= 0 && x <= n && y >= 0 && y <= n && config[n*x+y] == EMPTY)
            {
                config[n*x+y] = DOT;
                Board board(config, -1, -1, DOT, 0, false);
                if (!board.eval(false))
                {
                    cout<<"DRAW\n";
                    return 0;
                }
                //cout<<endl<<board.getValue()<<endl;
                if (board.getValue() == INT_MIN+1)
                {
                    cout<<"You Win\n";
                    return 0;
                }
                short maxDepth = (n > 5 || s < 120) ? 3 : 5;
                Move myMove = getBestMove(board,board.getValue(), true, maxDepth);
                int mx = myMove.getX();
                int my = myMove.getY();
                if (mx == -1 && my == -1)
                    cout<<"I forfeit. Can't move.";
                else
                    config[n*mx + my] = CROSS;
                cout<<"My Move:"<<mx<<" , "<<my<<endl;
                print(config, first);
                Board newBoard(config, -1, -1, CROSS, 0, false);
                newBoard.eval(false);
                //cout<<endl<<newBoard.getValue()<<endl;
                if (newBoard.getValue() == INT_MAX)
                {
                    cout<<"I Win\n";
                    return 0;
                }
            }
            else
                cout<<"Invalid Move"<<endl;
            wannaPlay = 'n';
            cout << "Keep going?";
            cin>>wannaPlay;
        }
    }
    else if (mode == 2)
    {
        first = rand()%2 + 1;
        if (first == 2)
        {
            cout<<"\nToss result: I go first."<<endl;
            Board board(config, -1, -1, DOT, 0, false);
            int mx = n/2;
            int my = n/2;
            config[n*mx + my] = CROSS;
            cout<<"My Move:"<<mx<<" , "<<my<<endl;
            print(config, first);
        }
        else
        {
            CROSS = 'o';
            DOT = 'x';
            cout<<"\n Toss result: Random goes first."<<endl;
            cout<<"Do you want to continue? ";
            cin>>wannaPlay;
        }
        while (wannaPlay == 'y' || wannaPlay == 'Y')
        {
            while (true)
            {
                x = rand()%n;
                y = rand()%n;
                if (config[n*x+y] == EMPTY)
                    break;
            }
            t = clock();
            if (x >= 0 && x <= n && y >= 0 && y <= n && config[n*x+y] == EMPTY)
            {
                config[n*x+y] = DOT;
                cout<<"Random's Move:"<<x<<" , "<<y<<endl;
                print(config, first);
                cout<<endl<<endl;
                Board board(config, -1, -1, DOT, 0, false);
                board.eval(false);
                //cout<<endl<<board.getValue()<<endl;
                if (board.getValue() == INT_MIN+1)
                {
                    cout<<"You Win\n";
                    return 0;
                }
                Move myMove = getBestMove(board,board.getValue(), true, 3);
                int mx = myMove.getX();
                int my = myMove.getY();
                if (mx == -1 && my == -1)
                    cout<<"I forfeit. Can't move.";
                else
                    config[n*mx + my] = CROSS;
                cout<<"My Move:"<<mx<<" , "<<my<<endl;
                print(config, first);
                Board newBoard(config, -1, -1, CROSS, 0, false);
                newBoard.eval(false);
                //cout<<endl<<newBoard.getValue()<<endl;
                if (newBoard.getValue() == INT_MAX)
                {
                    cout<<"I Win\n";
                    return 0;
                }
            }
            else
                cout<<"Invalid Move"<<endl;
            wannaPlay = 'n';
            cout << "Keep going?";
            cin>>wannaPlay;
        }
    }
    else if (mode == 3)
    {
        bool init = true;
        char turn = CROSS;
        Board board(config, -1, -1, DOT, 0, false);
        if (init)
        {
            int mx = n/2;
            int my = n/2;
            config[n*mx + my] = CROSS;
            cout<<"CROSS's move: "<<mx<<" , "<<my<<endl;
            print(config, 2);
            turn = DOT;
            init = false;
        }
        while (wannaPlay == 'y' || wannaPlay == 'Y')
        {
            t = clock();
            char c = (turn == CROSS) ? DOT : CROSS;
            CROSS = turn;
            DOT = c;
            Board board(config, -1, -1, DOT, 0, false);
            board.eval(false);
            Move myMove = getBestMove(board,board.getValue(), true, 3);
            int mx = myMove.getX();
            int my = myMove.getY();
            if (mx == -1 && my == -1)
                cout<<"I forfeit. Can't move.";
            else
                config[n*mx + my] = CROSS;
            cout<<"My Move:"<<mx<<" , "<<my<<" , "<<myMove.getType()<<endl;
            print(config, 2);
            Board newBoard(config, -1, -1, CROSS, 0, false);
            if (!newBoard.eval(false))
            {
                cout<<"DRAW\n";
                return 0;
            }
            if (newBoard.getValue() == INT_MAX)
            {
                if (CROSS == 'x')
                    cout<<"CROSS Wins\n";
                else
                    cout<<"DOT Wins\n";
                return 0;
            }
            else if (newBoard.getValue() == INT_MIN + 1)
            {
                if (CROSS == 'x')
                    cout<<"DOT Wins\n";
                else
                    cout<<"CROSS Wins\n";
            }
            //cout<<endl<<newBoard.getValue()<<endl;
            turn = c;
        }
    }
    return 0;
}

Move abminimax(Board& board, short, short, int, int, bool,int, bool);

Move getBestMove(Board& board,int rootVal, bool useTT, short maxDepth)
{
	short depth = 0;
	Move mnew, mold;
	while (depth < maxDepth)
	{
        mnew = abminimax(board, depth, 0, INT_MIN, INT_MAX, true, rootVal, useTT);
        if (mnew.getX() == n+1 && mnew.getY() == n+1)
            break;
        if (depth % 2 == 0)
            mold = mnew;
        depth++;
	}
	return mold;
}

//MINIMAX PROCEDURE
Move abminimax(Board& state, short depth, short height, int a, int b, bool maxPlayer, int rootVal, bool useTT)
{
    Move m;
    int deeper = 0;
    if (getTimeRemaining() < 5)
        return (Move(n+1, n+1, CROSS));
    if (depth == 0 || height >= 3 || state.isTerminal())// || timeRem < 0.03 * s)
    {
        if (!useTT)
            state.eval(useTT);
        return state.getMove();
    }
    if (maxPlayer)
    {
        vector<Board> succ = state.getSuccessors(useTT);
        for (int i = succ.size() - 1; i >= 0; i--)
        {
            abminimax(succ[i], depth-1, height +1, a, b, false, rootVal, useTT);
            if (succ[i].getValue() > a)
            {
                a = succ[i].getValue();
                state.setValue(a);
                std::unordered_map<Board, int, myHash>::const_iterator res = transTable.find(state);
				if (res != transTable.end())
                {
                    transTable.erase(res);
                    std::pair<Board, int> hashEntry(state, state.getValue());
                    transTable.insert(hashEntry);
                }
                m = succ[i].getMove();
            }
            if (a >= b)
            {
                break;
            }
        }
    }
    else
    {
        vector<Board> succ = state.getSuccessors(useTT);
        for (int i = 0; i < succ.size(); i++)
        {
            abminimax(succ[i], depth-1, height + 1, a, b, true, rootVal, useTT);
            if (succ[i].getValue() < b)
            {
                b = succ[i].getValue();
                state.setValue(b);
                std::unordered_map<Board, int, myHash>::const_iterator res = transTable.find(state);
				if (res != transTable.end())
                {
                    transTable.erase(res);
                    std::pair<Board, int> hashEntry(state, state.getValue());
                    transTable.insert(hashEntry);
                }
                m = succ[i].getMove();
            }

            if (a >= b)
            {
                break;
            }
        }
    }
    return m;
}

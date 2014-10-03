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
#include <cctype>

using namespace std;

class Symbol
{
    public:
        Symbol()
        {
            Symbol("", -1, true);
        }
        Symbol(string name, int id, bool positive)
        {
            m_name = name;
            m_id = id;
            m_positive = positive;
        }
        ~Symbol()
        {

        }

        int getId()
        const{
            return m_id;
        }

        string getName()
        const{
            return m_name;
        }

        bool isPositive()
        const{
            return m_positive;
        }

        bool operator==(const Symbol& sym2)
        const {
            return sym2.getId() == this->m_id;
        }

    private:
        string m_name;
        int m_id;
        bool m_positive;
};

int sid = 0;
vector<Symbol> symbols;
int hcid = 0;

class HornClause
{
    public:
        HornClause()
        {
            m_head = Symbol();
            m_id = -1;
        }
        HornClause(vector<Symbol> premise, Symbol head, int id)
        {
            m_premise = premise;
            m_head = head;
            m_id = id;
        }
        ~HornClause()
        {

        }

        int getId()
        const{
            return m_id;
        }

        vector<Symbol> getPremise()
        const {
            return m_premise;
        }

        Symbol getHead()
        const {
            return m_head;
        }

        void print()
        {
            cout<<endl;
            for (vector<Symbol>::iterator it = m_premise.begin(); it != m_premise.end(); it++)
            {
                cout<<it->getName();
                if (it < m_premise.end() - 1)
                    cout<<" ^ ";
                else
                    cout<<" => ";
            }
            cout<<m_head.getName();
        }

        bool operator==(const HornClause& hc2)
        const {
            return hc2.getId() == this->m_id;
        }

    private:
        vector<Symbol> m_premise;
        Symbol m_head;
        int m_id;
};

bool backwardChaining(vector<HornClause>& kb, Symbol& q)
{
    cout<<"\nBackward chaining:";
    vector<bool> inferred(sid, false);
    vector<bool> hcUsed(hcid, false);
    vector<Symbol> goals;
    //stack<HornClause> steps;
    goals.push_back(q);
    while (!goals.empty())
    {
        bool goalsModified = false;
        for (vector<HornClause>::iterator it = kb.begin(); it != kb.end(); it++)
        {
            Symbol head = it->getHead();
            vector<Symbol> premise = it->getPremise();
            vector<Symbol>::iterator goal = find(goals.begin(), goals.end(), head);
            if (goal != goals.end())
            {
                if (!hcUsed[it->getId()])
                {
                    it->print();
                    hcUsed[it->getId()] = true;
                }
                bool premiseInferred = true;
                for (vector<Symbol>::iterator sym = premise.begin(); sym != premise.end(); sym++)
                    if (!inferred[sym->getId()])
                    {
                        premiseInferred = false;
                        if (find(goals.begin(), goals.end(), *sym) == goals.end())
                        {
                            goalsModified = true;
                            goals.push_back(*sym);
                        }
                    }
                if (premiseInferred)
                {
                    if (head == q)
                    {
                        cout<<endl<<"--->true";
                        return true;
                    }
                    inferred[head.getId()] = true;

                    goals.erase(goal);

                    goalsModified = true;
                    break;
                }
            }
        }
        if (!goalsModified)
        {
            cout<<endl<<"--->false";
            return false;
        }
    }
    cout<<"--->false";
    return false;
}

bool forwardChaining(vector<HornClause>& kb, Symbol& q)
{
    cout<<"\nForward chaining:";
    vector<int> count(kb.size());
    queue<Symbol> agenda;
    for (vector<HornClause>::iterator it = kb.begin(); it != kb.end(); it++)
    {
        int premiseSize = it->getPremise().size();
        count[it->getId()] = premiseSize;
        if (premiseSize == 0)
        {
            cout<<endl<<it->getHead().getName();
            if (it->getHead() == q)
            {

                //cout<<endl<<it->getHead().getId()<<" : "<<q.getId()<<"--->true";
                cout<<"\n--->true";
                return true;
            }
            agenda.push(it->getHead());
        }
    }
    vector<bool> inferred(sid, false);
    while (!agenda.empty())
    {
        Symbol p = agenda.front();
        agenda.pop();
        if (!inferred[p.getId()])
        {
            inferred[p.getId()] = true;
            for (vector<HornClause>::iterator it = kb.begin(); it != kb.end(); it++)
            {
                vector<Symbol> premise = it->getPremise();
                if (std::find(premise.begin(), premise.end(), p) != premise.end())
                {
                    count[it->getId()]--;
                    if (count[it->getId()] == 0)
                    {
                        it->print();
                        if (it->getHead() == q)
                        {

                            //cout<<endl<<it->getHead().getId()<<" : "<<q.getId()<<"--->true";
                            cout<<"\n--->true";
                            return true;
                        }
                        agenda.push(it->getHead());
                    }
                }
            }
        }
    }
    cout<<endl<<"--->false";
    return false;
}

/*vector<string> split(const string &s, char delim) {
    vector<string> elems;
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}*/


/*HornClause transformToHornClause(string txt)
{
    int i = 0;
    vector<Symbol> body;
    Symbol head;
    while (i < txt.size())
    {
        switch (line[i])
        {
            case ' ':
               i++;
               break;

            case '^':
                i++;
                break;
            default:
                if (line[i] == '~' || isalnum(line[i]))
                {
                    string txt;
                    while (i < line.size())
                        txt += line[i++];
                    kb.push_back(transformToHornClause(txt));
                }
                else
                    cout<<"Invalid input: "<<line;
        }
    }


}

void parseInput()
{
    vector<HornClause> kb;
    std::ifstream kbfile(filename);
    if (kbfile.is_open())
    {

        getline(kbfile, line);
        int i = 0;
        while (getline(kbfile, line))
        {
            cout<<line<<endl;
            while (i < line.size())
                switch (line[i])
                {
                    case '(':
                       i++;
                       string txt;
                       while (line [i] != ')')
                           txt += line[i++];
                       i++;
                       kb.push_back(transformToHornClause(txt));
                       break;
                    case ' ':
                        i++;
                        break;
                    case '^':
                        i++;
                        break;
                    default:
                        if (line[i] == '~' || isalnum(line[i]))
                        {
                            string txt;
                            while (i < line.size())
                                txt += line[i++];
                            kb.push_back(transformToHornClause(txt));
                        }
                        else
                            cout<<"Invalid input: "<<line;
                }
        }
    }
    if (kbfile.is_open())
    {
        getline(kbfile, line);

        vector <>

    kbfile.close();
}*/

void print(vector<Symbol> &clause)
{
    cout<<"(";
    for (vector<Symbol>::iterator it = clause.begin(); it != clause.end(); it++)
    {
        if (!it->isPositive())
            cout<<"~";
        cout<<it->getName();
        if (clause.size() > 1 && it < clause.end() - 1)
            cout<<" v ";
    }
    cout<<")";

}

void negateSentence(vector<vector<Symbol> > &query)
{

}

bool resolve(vector<Symbol> &clause1, vector<Symbol> &clause2, vector<vector<Symbol> > &newClauses)
{
    for (vector<Symbol>::iterator symbol1 = clause1.begin(); symbol1 != clause1.end(); symbol1++)
    {
        bool compFound = false;
        for (vector<Symbol>::iterator symbol2 = clause2.begin(); symbol2 != clause2.end(); symbol2++)
            if ((*symbol1 == *symbol2) && (symbol1->isPositive() != symbol2->isPositive()))
            {
                compFound = true;
                vector<Symbol> merged;
                merged.insert(merged.end(), clause1.begin(), symbol1);
                merged.insert(merged.end(), symbol1 + 1, clause1.end());
                //merged.insert(merged.end(), clause2.begin(), symbol2);
                //merged.insert(merged.end(), symbol2 + 1, clause2.end());
                bool isNewClauseAlwaysTrue = false;
                for (vector<Symbol>::iterator it = clause2.begin(); it != clause2.end(); it++)
                {
                    vector<Symbol>::iterator index = find(merged.begin(), merged.end(), *it);
                    if (index != merged.end())
                    {
                        if (index->isPositive() != it->isPositive())
                        {
                            isNewClauseAlwaysTrue = true;
                            break;
                        }

                    }
                    else if (it != symbol2)
                        merged.push_back(*it);
                }
                if (merged.size() == 0)
                {
                    cout<<endl<<"Resolve ";
                    print(clause1);
                    cout<<" and ";
                    print(clause2);
                    cout<<" ---> null";
                    return true;
                }

                if (!isNewClauseAlwaysTrue)
                {
                    cout<<endl<<"Resolve ";
                    print(clause1);
                    cout<<" and ";
                    print(clause2);
                    cout<<" ---> ";
                    print(merged);
                    newClauses.push_back(merged);
                }
                return false;
            }
    }
    return false;
}

bool compareClauses(vector<Symbol> &c1, vector<Symbol> &c2)
{
    /*cout<<endl<<"Compare: ";
    print(c1);
    cout<<" and ";
    print(c2);*/
    if (c1.size() != c2.size())
        return false;
    for (vector<Symbol>::iterator it1 = c1.begin(); it1 != c1.end(); it1++)
    {
        bool curFound = false;
        for (vector<Symbol>::iterator it2 = c2.begin(); it2 != c2.end(); it2++)
            if ((*it2 == *it1) && (it1->isPositive() == it2->isPositive()))
            {
                curFound = true;
                break;
            }
        if (!curFound)
            return false;
    }
    return true;
}

bool mergeClauses(vector<vector<Symbol> > &newClauses, vector<vector<Symbol> > &clauses)
{
    if (newClauses.size() == 0)
        return true;
    bool isSubsetOf = true;
    for (vector<vector<Symbol> >::iterator nit = newClauses.begin(); nit != newClauses.end(); nit++)
    {
        bool add = true;
        for (vector<vector<Symbol> >::iterator cit = clauses.begin(); cit != clauses.end(); cit++)
            if (compareClauses(*nit, *cit))
            {
                add = false;
                break;
            }
        if (add)
        {
            isSubsetOf = false;
            clauses.push_back(*nit);
        }
    }
    return isSubsetOf;
}

bool resolution(const vector<vector<Symbol> > &kb, vector<vector<Symbol> > &query)
{
    //negateSentence(query);
    cout<<"\nResolution:";
    vector<vector<Symbol> > clauses(kb);
    clauses.insert(clauses.end(), query.begin(), query.end());
    int x = 0;
    while (true)//(x < 2)
    {
        vector<vector<Symbol> > newClauses;
        for (int i = 0; i < clauses.size() - 1; i++)
        {
            for (int j = i+1; j < clauses.size(); j++)
            {
                if (resolve(clauses[i], clauses[j], newClauses))
                {
                    cout<<"\n--> true";
                    return true;
                }
            }
        }
        if (mergeClauses(newClauses, clauses))
        {
            cout<<"\nNo new clauses to add --> false";
            return false;
        }
        //x++;
    }
    cout<<"\n--> false";
    return false;
}

Symbol getUniqueSymbol(string symbol, bool positive)
{
    for (int i = 0; i < symbols.size(); i++)
        if (symbol == symbols[i].getName())
        {
            //cout<<"Same sym found"<<symbol<<" : "<<symbols[i].getName()<<endl;
            if (symbols[i].isPositive() == positive)
                return symbols[i];
            else
                return Symbol(symbol, symbols[i].getId(), positive);
        }
    //cout<<"New symbol: ";
    //for (int i = 0; i < symbol.length(); i++)
    //    cout<<symbol[i];
    //cout<<":"<<symbol.length()<<endl;
    Symbol s(symbol, sid++, positive);
    symbols.push_back(s);
    return s;
}

void addHornClause(string clause, vector<HornClause> &kb)
{
	bool conclusion = false;
	vector<Symbol> premise;
	Symbol head;
	int x = 0;
	while (clause[x] == ' ')
		x++;

	for (int j = x; j < clause.size(); j++)
	{
		string symbol;
		while(clause[j] != ' ' && clause[j] != '^' && clause[j] != '\n' && j < clause.size())
		{
		    if (clause[j+1] == '=')
			{
			    symbol.push_back(clause[j]);
			    break;
			}
			symbol.push_back(clause[j]);
			if(symbol == "=>")
                break;
			j++;
		}
		if(symbol == "=>")
			conclusion = true;
		else if(conclusion && symbol != "")
		{
		    head = getUniqueSymbol(symbol, true);
		}
		else if (symbol != "")
		{
			premise.push_back(getUniqueSymbol(symbol, true));
		}

	}
	if (conclusion == false && premise.size() == 1)
	{
		head = premise[0];
		premise.clear();
	}
	HornClause hc(premise, head, hcid++);
	kb.push_back(hc);
	cout<<endl;
	cout<<"New clause added: ";
	hc.print();
	cout<<endl;
}

void convertToHornAndAdd(string clause, vector<HornClause> &kb)
{
    vector<Symbol> premise;
	Symbol head;
	int x = 0;
	while (clause[x] == ' ')
		x++;

    for(int j = x; j < clause.size(); j++)
	{
		string symbol;
		bool prem = false;
		while(clause[j] != ' ' && clause[j] != 'v' && j < clause.size())
		{
			if(clause[j] == '~')
			{
				prem = true;
				j++;
			}
			symbol.push_back(clause[j]);
			j++;
		}
		if (symbol != "")
		{
		    if (prem)
			premise.push_back(getUniqueSymbol(symbol, true));
            else
                head = getUniqueSymbol(symbol, true);
		}
	}
	HornClause hc(premise, head, hcid++);
	kb.push_back(hc);
	cout<<endl;
	if (hc.getPremise().size() > 0)
        cout<<"Converting clause: "<<clause;
	cout<<"\nNew clause added: ";
	hc.print();
	cout<<endl;
}

bool isImplication(string line)
{
	int nbP = 0;
	for(int j = 0; j<line.size(); j++)
	{
		if (line[j] == '(')
			nbP++;
		if (line[j] == ')')
			nbP--;
		if(line[j] == '=')
		{
			if(line[j+1] == '>' && nbP == 0)
			{
			    //cout<<endl<<"Implication";
			    return true;
			}

		}
	}
	return false;
}

bool isDisjunction(string line)
{
	int nbP = 0;
	for(int j = 0; j<line.size(); j++)
	{
		if (line[j] == '(')
			nbP++;
		if (line[j] == ')')
			nbP--;

		if (line[j] == 'v' && nbP == 0)
		{
		    //cout<<endl<<"Disjucntion";
			return true;
		}
	}
	return false;
}

vector<HornClause> getChainingKB(const char *filename)
{
    vector<HornClause> knowledgeBase;
    vector<Symbol> premise;
    std::ifstream kbfile(filename);
    if (kbfile.is_open())
    {
        string line;
        while (getline(kbfile, line))
        {
            if (isImplication(line))
				addHornClause(line, knowledgeBase);
			else if (isDisjunction(line))
				convertToHornAndAdd(line, knowledgeBase);
			else
			{
				for(int j = 0; j < line.size(); j++)
				{
					if(line[j]=='(')
					{
						std::string cur;
						j++;
						int p = 0;
						while(line[j] != ')' || p > 0)
						{
							std::string s(1, line[j]);
							cur += s;
							if(s == "(")
								p++;
							else if(s == ")")
								p--;
							j++;
						}
						if (isImplication(cur))
							addHornClause(cur, knowledgeBase);
						else
							convertToHornAndAdd(cur, knowledgeBase);
					}
					else if (line[j] != '^' && line[j] != ' ')
					{
						std::string cur;
						while(line[j] != ' ' && line[j] != '^' && j < line.size())
						{
							std::string s(1, line[j]);
							cur += s;
							j++;
						}
						addHornClause(cur, knowledgeBase);
					}
				}
			}
        }
    }
    kbfile.close();

    /*premise.push_back(Symbol("P", sid++, true));
    knowledgeBase.push_back(HornClause(premise, Symbol("Q", sid++, true), hcid++));
    premise.clear();
    premise.push_back(Symbol("L", sid++, true));
    premise.push_back(Symbol("M", sid++, true));
    knowledgeBase.push_back(HornClause(premise, Symbol("P", 0, true), hcid++));
    premise.clear();
    premise.push_back(Symbol("B", sid++, true));
    premise.push_back(Symbol("L", 2, true));
    knowledgeBase.push_back(HornClause(premise, Symbol("M", 3, true), hcid++));
    premise.clear();
    premise.push_back(Symbol("A", sid++, true));
    premise.push_back(Symbol("P", 0, true));
    knowledgeBase.push_back(HornClause(premise, Symbol("L", 2, true), hcid++));
    premise.clear();
    premise.push_back(Symbol("A", 4, true));
    premise.push_back(Symbol("B", 5, true));
    knowledgeBase.push_back(HornClause(premise, Symbol("L", 2, true), hcid++));
    premise.clear();
    knowledgeBase.push_back(HornClause(premise, Symbol("A", 4, true), hcid++));
    premise.clear();
    knowledgeBase.push_back(HornClause(premise, Symbol("B", 5, true), hcid++));*/

    return knowledgeBase;
}

void addCNF(string clause, vector<vector<Symbol> > &kb)
{
    vector<Symbol> cnf;
	int x = 0;
	while (clause[x] == ' ')
		x++;

    for(int j = x; j < clause.size(); j++)
	{
		string symbol;
		bool neg = false;
		while(clause[j] != ' ' && clause[j] != 'v' && j < clause.size())
		{
			if(clause[j] == '~')
			{
				neg = true;
			}
			else
                symbol.push_back(clause[j]);
			j++;
		}
		if (symbol != "")
		{
		    cnf.push_back(getUniqueSymbol(symbol, !neg));
		}
	}
	kb.push_back(cnf);
	cout<<endl;
	cout<<"\nNew clause added: ";
	print(cnf);
	cout<<endl;
}

vector<vector<Symbol> > getCNFKB(const char* filename)
{
    vector<vector<Symbol> > knowledgeBase;
    std::ifstream kbfile(filename);
    if (kbfile.is_open())
    {
        string line;
        while (getline(kbfile, line))
        {
            if (isDisjunction(line))
				addCNF(line, knowledgeBase);
			else
			{
				for(int j = 0; j < line.size(); j++)
				{
					if(line[j]=='(')
					{
						std::string cur;
						j++;
						int p = 0;
						while(line[j] != ')' || p > 0)
						{
							std::string s(1, line[j]);
							cur += s;
							if(s == "(")
								p++;
							else if(s == ")")
								p--;
							j++;
						}
						addCNF(cur, knowledgeBase);
					}
					else if (line[j] != '^' && line[j] != ' ')
					{
						std::string cur;
						while(line[j] != ' ' && line[j] != '^' && j < line.size())
						{
							std::string s(1, line[j]);
							cur += s;
							j++;
						}
						addCNF(cur, knowledgeBase);
					}
				}
			}
        }
    }
    kbfile.close();

    /*vector<Symbol> clause;
    clause.push_back(Symbol("B", sid++, false));
    clause.push_back(Symbol("P", sid++, true));
    clause.push_back(Symbol("M", sid++, true));
    kb.push_back(clause);
    clause.clear();
    clause.push_back(Symbol("B", 0, true));
    clause.push_back(Symbol("P", 1, false));
    kb.push_back(clause);
    clause.clear();
    clause.push_back(Symbol("B", 0, true));
    clause.push_back(Symbol("M", 2, false));
    kb.push_back(clause);
    clause.clear();
    clause.push_back(Symbol("B", 0, false));
    kb.push_back(clause);*/
    return knowledgeBase;
}

int main(int argc, char** argv)
{
    if (argc < 4)
    {
        cout<<"Insufficient arguments";
        cout<<endl;
    }
    sid = 0;
    if (string(argv[1]) == "forward")
    {
        hcid = 0;
        Symbol query = getUniqueSymbol(string(argv[3]), true);
        vector<HornClause> knowledgeBase = getChainingKB(argv[2]);
        //cout<<endl<<symbols.size();
        //for (int i = 0; i < symbols.size(); i++)
        //    cout<<endl<<symbols[i].getName();
        //cout<<endl;
        forwardChaining(knowledgeBase, query);
        cout<<endl;
        return 0;
    }
    else if (string(argv[1]) == "backward")
    {
        hcid = 0;
        Symbol query = getUniqueSymbol(string(argv[3]), true);
        vector<HornClause> knowledgeBase = getChainingKB(argv[2]);
        //cout<<endl<<symbols.size();
        //for (int i = 0; i < symbols.size(); i++)
        //    cout<<endl<<symbols[i].getName();
        //cout<<endl;
        backwardChaining(knowledgeBase, query);
        cout<<endl;
        return 0;
    }
    else if (string(argv[1]) == "CNF")
    {
        //TODO handle properly
        bool querySign = false;
        char *queryStr = argv[3];
        if (argv[3][0] == '~')
        {
            queryStr++;
            querySign = true;
        }
        string q = string(queryStr);
        Symbol query = getUniqueSymbol(q, querySign);
        vector<vector<Symbol> > kb = getCNFKB(argv[2]);
        vector<Symbol> queryClause;
        queryClause.push_back(query);
        vector<vector<Symbol> > queryClauses;
        queryClauses.push_back(queryClause);
        /*cout<<endl<<symbols.size();
        for (int i = 0; i < symbols.size(); i++)
            cout<<endl<<symbols[i].getName()<<":"<<symbols[i].getId();
        cout<<endl;*/
        cout<<"\nClauses in the query (after negation):"<<endl;
        for (vector<vector<Symbol> >::iterator it = queryClauses.begin(); it != queryClauses.end(); it++)
        {
            print(*it);
            cout<<endl;
        }
        resolution(kb, queryClauses);
        cout<<endl;
        return 0;
    }
    else
    {
        cout<<"\nWrong entailment mode: Please enter one of 'backward', 'forward' or 'CNF'.\n";
        return -1;
    }

    /*hcid = 0;
    Symbol query = getUniqueSymbol(string(argv[3]), true);
    vector<HornClause> knowledgeBase = getChainingKB(argv[2]);
    cout<<endl<<symbols.size();
    for (int i = 0; i < symbols.size(); i++)
        cout<<endl<<symbols[i].getName();
    //forwardChaining(knowledgeBase, query);
    backwardChaining(knowledgeBase, query);
    //vector<vector<Symbol> > cnfq;
    //vector<Symbol> cnfqclause;
    //Symbol s("R", 3, true);
    //Symbol s("P", 1, true);
    //cnfqclause.push_back(s);
    //cnfq.push_back(cnfqclause);
    //vector<vector<Symbol> > cnfkb = getCNFKB();
    //resolution(cnfkb, cnfq);
    //vector<vector<Symbol> > empty;
    //resolve(cnfkb[3], cnfqclause, empty);
    cout<<endl;*/
}

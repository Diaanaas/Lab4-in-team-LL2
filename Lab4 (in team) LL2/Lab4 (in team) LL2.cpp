#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <stack>

using namespace std;

const string FILE_NAME = "./input.txt";

void find_first(vector< pair<string, string> > gram,
    map< string, set<string> >& firsts,
    string non_term);

void find_follow(vector< pair<string, string> > gram,
    map< string, set<string> >& follows,
    map< string, set<string> > firsts,
    string non_term);


int main()
{
    cout << "Input string: ";
    string input_string;
    cin >> input_string;
    fstream grammar_file;
    grammar_file.open(FILE_NAME, ios::in);
    if (grammar_file.fail())
    {
        cout << "Error in opening grammar file\n";
        system("pause");
        return 2;
    }

    cout << "Grammar parsed from grammar file: \n";
    vector< pair<string, string> > gram;
    int count = 0;
    while (!grammar_file.eof())
    {
        char buffer[20];
        grammar_file.getline(buffer, 19);
        char l = buffer[0];
        string lhs = "";
        lhs.push_back(l);
        string rhs = buffer + 3;
        pair <string, string> prod(lhs, rhs);
        gram.push_back(prod);
        cout << count++ << ".  " << gram.back().first << " -> " << gram.back().second << "\n";
    }
    cout << "\n";
    set<string> non_terms;
    for (auto i = gram.begin(); i != gram.end(); ++i)
    {
        non_terms.insert(i->first);
    }
    cout << "The non terminals in the grammar are: ";
    for (auto i = non_terms.begin(); i != non_terms.end(); ++i)
    {
        cout << *i << " ";
    }
    cout << "\n";
    set<string> terms;
    for (auto i = gram.begin(); i != gram.end(); ++i)
    {
        terms.insert(i->second);
    }
    terms.erase("e");
    terms.insert("$");
    cout << "The terminals in the grammar are: ";
    for (auto i = terms.begin(); i != terms.end(); ++i)
    {
        cout << *i << " ";
    }
    cout << "\n\n";

    string start_sym = gram.begin()->first;


    map< string, set<string> > firsts;
    for (auto non_term = non_terms.begin(); non_term != non_terms.end(); ++non_term)
    {
        if (firsts[*non_term].empty())
        {
            find_first(gram, firsts, *non_term);
        }
    }

    cout << "Firsts list: \n";
    for (auto it = firsts.begin(); it != firsts.end(); ++it)
    {
        cout << it->first << " : ";
        for (auto firsts_it = it->second.begin(); firsts_it != it->second.end(); ++firsts_it)
        {
            cout << *firsts_it << " ";
        }
        cout << "\n";
    }
    cout << "\n";


    map< string, set<string> > follows;

    string start_var = gram.begin()->first;
    follows[start_var].insert("$");
    find_follow(gram, follows, firsts, start_var);

    for (auto it = non_terms.begin(); it != non_terms.end(); ++it)
    {
        if (follows[*it].empty())
        {
            find_follow(gram, follows, firsts, *it);
        }
    }

    cout << "Follows list: \n";
    for (auto it = follows.begin(); it != follows.end(); ++it)
    {
        cout << it->first << " : ";
        for (auto follows_it = it->second.begin(); follows_it != it->second.end(); ++follows_it)
        {
            cout << *follows_it << " ";
        }
        cout << "\n";
    }
    cout << "\n";


    int parse_table[100][100];
    fill(&parse_table[0][0], &parse_table[0][0] + sizeof(parse_table) / sizeof(parse_table[0][0]), -1);
    for (auto prod = gram.begin(); prod != gram.end(); ++prod)
    {
        string rhs = prod->second;
        set<string> next_list;
        bool finished = false;
        for (auto ch = rhs.begin(); ch != rhs.end(); ch++)
        {
            string res = ""; res += *ch;
            ch++;
            res += *ch;
            if (!isupper(*ch))
            {
                if (*ch != 'e')
                {
                    next_list.insert(res);
                    finished = true;
                    break;
                }
                continue;
            }

            set<string> firsts_copy(firsts[res].begin(), firsts[res].end());
            if (firsts_copy.find("e") == firsts_copy.end())
            {
                next_list.insert(firsts_copy.begin(), firsts_copy.end());
                finished = true;
                break;
            }
            firsts_copy.erase("e");
            next_list.insert(firsts_copy.begin(), firsts_copy.end());
        }
        if (!finished)
        {
            next_list.insert(follows[prod->first].begin(), follows[prod->first].end());
        }
        for (auto ch = next_list.begin(); ch != next_list.end(); ++ch)
        {
            int row = distance(non_terms.begin(), non_terms.find(prod->first));
            int col = distance(terms.begin(), terms.find(*ch));
            int prod_num = distance(gram.begin(), prod);
            if (parse_table[row][col] != -1)
            {
                cout << "Collision at [" << row << "][" << col << "] for production " << prod_num << "\n";
                continue;
            }
            parse_table[row][col] = prod_num;
            parse_table[row][0] = 0;
        }

    }
    cout << "Parsing Table: \n";
    cout << "   ";
    for (auto i = terms.begin(); i != terms.end(); ++i)
    {
        cout << *i << " ";
    }
    cout << "\n";
    for (auto row = non_terms.begin(); row != non_terms.end(); ++row)
    {
        cout << *row << "  ";
        for (int col = 0; col < terms.size(); ++col)
        {
            int row_num = distance(non_terms.begin(), row);
            if (parse_table[row_num][col] == -1)
            {
                cout << "- ";
                continue;
            }
            cout << parse_table[row_num][col] << " ";
        }
        cout << "\n";
    }
    cout << "\n";

    stack<string> st;
    st.push("$$");
    st.push("S");

    for (auto ch = input_string.begin(); ch != input_string.end(); ++ch)
    {
        string res = ""; res += *ch;
        ch++;
        res += *ch;
        if (terms.find(res) == terms.end())
        {
            cout << "Input string is invalid\n";
            system("pause");
            return 2;
        }
    }
    input_string.push_back('$');
    input_string.push_back('$');

    bool accepted = true;
    while (!st.empty() && !input_string.empty())
    {
        string stack_top = st.top() + "";
        char ch0 = input_string[0];
        char ch1 = input_string[1];
        string inversedPart = "";
        inversedPart.push_back(ch1);
        inversedPart.push_back(ch0);
        string normalPart = "";
        normalPart.push_back(ch0);
        normalPart.push_back(ch1);
        if (inversedPart == stack_top)
        {
            st.pop();
            input_string.erase(0, 2);
        }
        else
        {
            int row = distance(non_terms.begin(), non_terms.find(stack_top));
            int col = distance(terms.begin(), terms.find(normalPart));
            int prod_num = parse_table[row][col];

            if (prod_num == -1)
            {
                cout << "No production found in parse table\n";
                accepted = false;
                break;
            }

            st.pop();
            string rhs = gram[prod_num].second;
            if (rhs[0] == 'e')
            {
                continue;
            }
            for (auto ch = rhs.rbegin(); ch != rhs.rend(); ++ch)
            {
                string res = ""; res += *ch;
                ch++;
                res += *ch;
                st.push(res);
            }
        }
    }

    if (accepted)
    {
        cout << "Input string is accepted\n";
    }
    else
    {
        cout << "Input string is rejected\n";
    }
    system("pause");
    return 0;
}

void find_first(vector< pair<string, string> > gram,
    map< string, set<string> >& firsts,
    string non_term)
{

    // cout<<"Finding firsts of "<<non_term<<"\n";

    for (auto it = gram.begin(); it != gram.end(); ++it)
    {
        // Find productions of the non terminal
        if (it->first != non_term)
        {
            continue;
        }

        // cout<<"Processing production "<<it->first<<"->"<<it->second<<"\n";

        string rhs = it->second;
        for (auto ch = rhs.begin(); ch != rhs.end(); ch += 2)
        {
            string res = "";
            res += *ch;
            ch++;
            res += *ch;
            if (!isupper(*ch))
            {
                firsts[non_term].insert(res);
                break;
            }
            else
            {
                if (firsts[res].empty())
                {
                    find_first(gram, firsts, res);
                }
                if (firsts[res].find("e") == firsts[res].end())
                {
                    firsts[non_term].insert(firsts[res].begin(), firsts[res].end());
                    break;
                }

                set<string> firsts_copy(firsts[res].begin(), firsts[res].end());

                if (ch + 1 != rhs.end())
                {
                    firsts_copy.erase("e");
                }

                firsts[non_term].insert(firsts_copy.begin(), firsts_copy.end());
            }
        }

    }
}

void find_follow(vector< pair<string, string> > gram,
    map< string, set<string> >& follows,
    map< string, set<string> > firsts,
    string non_term)
{

    // cout<<"Finding follow of "<<non_term<<"\n";

    for (auto it = gram.begin(); it != gram.end(); ++it)
    {

        bool finished = true;
        auto ch = it->second.begin();

        for (; ch != it->second.end(); ++ch)
        {
            string res = ""; res += *ch;
            ch++;
            res += *ch;
            if (res == non_term)
            {
                finished = false;
                break;
            }
        }
        if (ch != it->second.end() && ch + 1 != it->second.end()) {
            ++ch;
            ++ch;
        }

        for (; ch != it->second.end() && !finished; ++ch)
        {
            string res = ""; res += *ch;
            ch++;
            res += *ch;
            if (!isupper(*ch))
            {
                follows[non_term].insert(res);
                finished = true;
                break;
            }

            set<string> firsts_copy(firsts[res]);
            if (firsts_copy.find("e") == firsts_copy.end())
            {
                follows[non_term].insert(firsts_copy.begin(), firsts_copy.end());
                finished = true;
                break;
            }
            firsts_copy.erase("e");
            follows[non_term].insert(firsts_copy.begin(), firsts_copy.end());

        }


        if (ch == it->second.end() && !finished)
        {
            if (follows[it->first].empty())
            {
                find_follow(gram, follows, firsts, it->first);
            }
            follows[non_term].insert(follows[it->first].begin(), follows[it->first].end());
        }

    }

}

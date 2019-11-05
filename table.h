#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <unordered_set>
using namespace std;

class ValueSet {
    public:
    ValueSet (string n, int v, const vector<pair<int,int>>& e);
    string name; // a name for it (in order for print with undefined value)
    int value; // 0 if undefined, which acts as an unknown.
    vector<pair<int,int>> elements;
};

class DeduceTable {
    // algebraic table for deducing/generating
    public:
    int unknowns;
    int n;

    unordered_set<ValueSet*> sets;
    vector<vector<ValueSet*>> dtable;
    // invariance:
    // table[i,j] must always point to the valueset that contains it
    // the pointers in a Table must only be owned by itself

    int cursor_row;
    int cursor_col;

    void init(int nn);  // initiate a nn*nn table, 
                        // each entry with its own valueset.

    void add(int v);    // set value v to current cursor position
                        // ensure that the valueset at current position
                        // has undefined value (i.e. value=0)

    Table make_copy(); // make all memory allocation
    void freeSet(); // free all memory allocation (call it for destruction)
    
    void listAllTables(string filename); //

    string hashName(ValueSet* vs); // return a hashed name with length 3
    void print();

    // variables for performance inspection
    int call_depth; // the depth of its call stack
};

class Table {
    // value table for file I/O and property check
    public:
    Table(const DeduceTable& dt);

    int n;
    vector<vector<int>> table;

    friend ifstream& operator>> (ifstream& ifs, Table& t);
    friend ofstream& operator<< (ofstream& ofs, Table& t);

    bool checkAchievability();
    bool checkIdemPotence();
    bool checkCommutativity();
};
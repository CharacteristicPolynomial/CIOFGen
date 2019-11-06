#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <unordered_set>
#define FILE_PREFIX "CIOF_" 
#define FILE_SUFFIX ".result"
#define NAME_LENGTH 3
using namespace std;

class ValueSet {
    public:
    ValueSet (int v, const vector<pair<int,int>>& e);
    ValueSet(const ValueSet* vs);
    int minvalue();
    void add(int row, int col);
    void hashName(); // generate a random hashed name
    void addSet(ValueSet* vs);
    void changeSet(vector<vector<ValueSet*>>& dt, ValueSet* vs);
    string name; // a name for it (in order for print with undefined value)
    int value; // 0 if undefined, which acts as an unknown.
    // if value==0, then this is stored in undefined_sets
    // if value>0, then this is stored at defined_sets[value-1];
    vector<pair<int,int>> elements; // store the positions of the set elements
};

class DeduceTable {
    // algebraic table for deducing/generating
    public:
    int n;

    unordered_set<ValueSet*> undefined_sets; // undefined sets
    vector<ValueSet*> defined_sets; // value sets 1,2,...,n
    vector<vector<ValueSet*>> dtable;
    // invariance:
    // table[i,j] must always point to the valueset that contains it
    // the pointers in a Table must only be owned by itself

    int cursor_row;
    int cursor_col;

    int count;

    void init(int nn);  // initiate a nn*nn table, 
                        // each entry with its own valueset.

    bool assign(int v); // assign value to the current cursor // return false if not possible
    bool connect(int rowx, int colx, int rowy, int coly);     // return false if not possible
    bool setValue(int row, int col, int v);                   // return false if not possible
    int readValue(int row, int col);
    bool next(); // move cursor to its next position
    // return true when reaching the end;

    DeduceTable* make_copy(); // make all memory allocation
    void freeSets(); // free all memory allocation (call it for destruction)
    
    void fill_dtable();

    int listAllTables(string filename); // list all tables in filename
    // return the number of tables

    void print() const;
    void log(string filename) const;

    // variables for performance inspection
    int call_depth; // the depth of its call stack
};

class Table {
    // value table for file I/O and property check
    public:
    Table(const DeduceTable& dt);
    Table(int nn);

    int n;
    vector<vector<int>> table;

    friend istream& operator>> (istream& ifs, Table& t);
    friend ostream& operator<< (ostream& ofs, Table& t);

    void print();
    void check();
    bool checkAchievability();
    bool checkIdemPotence();
    bool checkCommutativity();
    bool checkMonotonicity();
};
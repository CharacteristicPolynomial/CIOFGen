#include <iostream>
#include <fstream>
#include "table.h"
using namespace std;

int main(int argc, const char* argv[]) {
    // format: ./checkDistinct [n] filename
    if(argc < 3) {
        cerr << "Missing argument. Format: ./checkDistinct [n] filename" << endl;
        exit(-1);
    }
    int n = atoi(argv[1]);
    Table t(n);
    vector<Table> vt;
    ifstream ifs;
    ifs.open(argv[2]);
    while(ifs >> t) {
        for(Table& t1 : vt) {
            if(t == t1) {
                cerr << "checkDistinct fails. Following matrix appears more than once:" << endl;
                t.print();
                ifs.close();
                exit(-1);
            }                
        }
        vt.push_back(t);
    }
    ifs.close();
    cout << "checkDistinct succeeds" << endl;
    return 0;
}
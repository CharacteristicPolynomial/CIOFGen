#include <iostream>
#include <fstream>
#include "table.h"
using namespace std;

int main(int argc, const char* argv[]) {
    // format: ./check [n] filename
    if(argc < 3) {
        cerr << "Missing argument. Format: ./check [n] filename" << endl;
        exit(-1);
    }
    int n = atoi(argv[1]);
    Table t(n);
    ifstream ifs;
    ifs.open(argv[2]);
    while(ifs >> t) {
        t.check();
    }
    ifs.close();
    return 0;
}
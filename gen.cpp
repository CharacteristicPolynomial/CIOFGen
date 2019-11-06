#include <iostream>
#include <fstream>
#include "table.h"
using namespace std;

int main(int argc, const char* argv[]) {
    // format: ./gen [n]
    if(argc < 2) {
        cerr << "Missing argument. Format: ./gen [n]" << endl;
        exit(-1);
    }
    DeduceTable dt;
    int n = atoi(argv[1]);
    dt.init(n);
    string filename = FILE_PREFIX + to_string(n) + FILE_SUFFIX;
    ofstream ofs;
    ofs.open(filename, ios_base::trunc);
    ofs.close();
    cout << "total number: " << dt.listAllTables(filename) << endl;
    dt.freeSets();
    return 0;
}
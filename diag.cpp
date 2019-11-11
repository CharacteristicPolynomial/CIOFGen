#include <iostream>
#include <string>
#include "table.h"
using namespace std;

int main(int argc, const char* argv[]) {
    // format: ./gen [n]
    if(argc < 3) {
        cerr << "Missing argument. Format: ./diag [n] filename" << endl;
        exit(-1);
    }
    int n = atoi(argv[1]);

    string Pfile = PFILE_PREFIX + to_string(n) + PFILE_SUFFIX;
    string QTfile = QTFILE_PREFIX + to_string(n) + QTFILE_SUFFIX;
    ofstream ofs;
    // initialize output files
    ofs.open(Pfile, ios_base::trunc);
    ofs.close();
    ofs.open(QTfile, ios_base::trunc);
    ofs.close();

    Table t(n);
    ifstream ifs;
    ifs.open(argv[2]);
    while(ifs >> t) {
        t.diagonalize(Pfile, QTfile);
    }
    ifs.close();
    return 0;
}
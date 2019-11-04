#include <iostream>
#include <fstream>
#define FILE_PREFIX "CIOF_" 
#define FILE_SUFFIX ".result"
using namespace std;

int main(int argc, const char* argv[]) {
    // format: ./gen [n]
    if(argc < 2) {
        cerr << "Missing argument. Format: ./gen [n]" << endl;
        exit(-1);
    }
    
    return 0;
}
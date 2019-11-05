#include "table.h"
#include <iostream>

ValueSet::ValueSet(string n, int v, const vector<pair<int,int>>& e) {
    name = n;
    value = v;
    elements = vector<pair<int,int>>(e);
}

void DeduceTable::init(int nn) {
    n = nn;
    // only call init when sets and dtable are empty
    if(!sets.empty() || !dtable.empty() ) {
        cerr << "Error: initializing non empty deduce table" << endl;
        exit(-1);
    }
    for(int i=0; i<n; i++) {
        // sets.
    }
}
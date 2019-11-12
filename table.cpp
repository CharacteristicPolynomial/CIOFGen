#include "table.h"
#include <iostream>
#include <functional>

ValueSet::ValueSet(int v, const vector<pair<int,int>>& e) {
    value = v;
    elements = vector<pair<int,int>>(e);
    if (value == 0) {
        hashName();
    } else {
        name = to_string(v);
    }
}

ValueSet::ValueSet(const ValueSet* vs) {
    name = vs->name;
    value = vs->value;
    elements = vector<pair<int,int>>(vs->elements);
}

int ValueSet::minvalue() {
    int temp = -1;
    for(auto e: elements) {
        if(temp < e.first)
            temp = e.first;
    }
    if (temp ==-1) {
        cerr << "Error: asking an empty valueset minvalue()" << endl;
        exit(-1);
    }
    return temp;
}

void ValueSet::add(int row, int col) {
    elements.push_back(make_pair(row, col));
}

void ValueSet::addSet(ValueSet* vs) {
    // append all elements in vs to itself
    for(auto e : vs->elements) {
        elements.push_back(e);
    }
}

void ValueSet::changeSet(vector<vector<ValueSet*>>& dt, ValueSet* vs) {
    // change all elements' pointer to vs
    for(auto e : elements) {
        dt[e.first][e.second] = vs;
    }
}

void ValueSet::hashName() {
    hash<ValueSet*> ptr_hash;
    size_t hashvalue = ptr_hash(this);
    name.resize(NAME_LENGTH);
    for(int i=0; i< NAME_LENGTH; i++) {
        // A:65 to Z:90
        int temp = hashvalue % 26;
        hashvalue /= 26;
        name[i] = temp + 65;
    }
}

void DeduceTable::init(int nn) {
    n = nn;
    count = 0;

    // only call init when sets and dtable are empty
    if(!undefined_sets.empty() || !dtable.empty()
        || !defined_sets.empty() ) {
        cerr << "Error: initializing non empty deduce table" << endl;
        exit(-1);
    }

    // allocating sets
    defined_sets.resize(n);
    for(int i=0; i<n; i++) {
        // fill defined sets
        vector<pair<int,int>> e;
        e.push_back(make_pair(0,i));
        ValueSet* temp = new ValueSet(i+1, e);
        defined_sets[i] = temp;
    }

    // allocates dtable
    for(int i=0; i<n; i++) {
        dtable.push_back(vector<ValueSet*>(n,NULL));
    }

    cursor_row = cursor_col = 0;

    call_depth = 0;

    // enforce achievability
    for(int i=0; i<n; i++) {
        if(assign(i+1)) {
            next();
        } else {
            cerr << "achievability fails" << endl;
            exit(-1);
        }
    }
}

bool DeduceTable::assign(int v) {
    if(setValue(cursor_row, cursor_col, v)) {
        // enforce idempotence
        if(!setValue(v-1, cursor_col, v))
            return false;
        // enforce commutativity
        for(int i=0; i<cursor_col; i++) {
            if(!connect(readValue(cursor_row,cursor_col)-1, i, readValue(cursor_row,i)-1, cursor_col))
                return false;
        }
        return true;
    } else {
        return false;
    }
}

bool DeduceTable::connect(int rowx, int colx, int rowy, int coly) {
    // cout << "connecting (" << rowx << ", " << colx << ") and " << "(" << rowy << ", " << coly << ")"
    // << endl;
    ValueSet*& xset = dtable[rowx][colx];
    ValueSet* xsetcopy = dtable[rowx][colx];
    ValueSet*& yset = dtable[rowy][coly];
    ValueSet* ysetcopy = dtable[rowy][coly];

    if(xset == NULL && yset == NULL) {
    // if both are undefined
        vector<pair<int,int>> e;
        e.push_back(make_pair(rowx, colx));
        e.push_back(make_pair(rowy, coly));
        ValueSet* newset = new ValueSet(0, e);
        undefined_sets.insert(newset);
        xset = yset = newset;
        return true;
    }

    if(xset == NULL && yset != NULL) {
        xset = yset;
        yset->add(rowx,colx);
        return true;
    }

    if(xset != NULL && yset == NULL) {
        yset = xset;
        xset->add(rowx,colx);
        return true;
    }

    if(xset == yset) {
        // they are one set
        return true;
    }

    // the case both has valuesets
    if(xset->value == 0) {
        // the case xset is undefined
        yset->addSet(xset);
        xset->changeSet(dtable, yset);
        if(undefined_sets.erase(xsetcopy) == 0) {
            cerr << "erasing non existing set in undefined_sets" << endl;
            exit(-1);
        }
        delete xsetcopy;
        return true;
    } else if(yset->value == 0) {
        // the case xset is undefined
        xset->addSet(yset);
        yset->changeSet(dtable, xset);
        if(undefined_sets.erase(ysetcopy) == 0) {
            cerr << "erasing non existing set in undefined_sets" << endl;
            exit(-1);
        }
        delete ysetcopy;
        return true;
    } else {
        return xset->value == yset->value;
    }
}

bool DeduceTable::setValue(int row, int col, int v) {
    ValueSet*& vs = dtable[row][col];
    ValueSet* vscopy = dtable[row][col];
    ValueSet* dv = defined_sets[v-1];
    if(vs == NULL) {
        vs = dv;
        vs->add(row, col);
        return true;
    } else if (vs->value == 0)  {
        dv->addSet(vs);
        vs->changeSet(dtable, dv);
        undefined_sets.erase(vscopy);
        delete vscopy;
        return true;
    } else {
        return v == vs->value;
    }
}

int DeduceTable::readValue(int row, int col) {
    ValueSet* vscopy = dtable[row][col];
    if (vscopy == NULL) {
        cerr << "reading null pointer" << endl;
        exit(-1);
    }
    if (vscopy->value == 0) {
        cerr << "reading undefined entry" << endl;
        exit(-1);
    }
    return vscopy->value;
}

bool DeduceTable::next() {
    if(cursor_col == n-1) {
        cursor_col = 0;
        cursor_row++;
    } else {
        cursor_col ++;
    }
    return cursor_row == n;
}

DeduceTable* DeduceTable::make_copy() {
    DeduceTable* newtable = new DeduceTable;
    newtable-> n = n;
    newtable-> cursor_row = cursor_row;
    newtable-> cursor_col = cursor_col;
    for(ValueSet* vs : undefined_sets) {
        (newtable->undefined_sets).insert(new ValueSet(vs));
    }
    for(ValueSet* vs : defined_sets) {
        (newtable->defined_sets).push_back(new ValueSet(vs));
    }
    newtable->fill_dtable();
    newtable->call_depth = call_depth+1;
    return newtable;
}

void DeduceTable::fill_dtable() {
    // allocates dtable
    for(int i=0; i<n; i++) {
        dtable.push_back(vector<ValueSet*>(n,NULL));
    }
    for(ValueSet* vs : undefined_sets) {
        for(auto e : vs->elements) {
            dtable[e.first][e.second] = vs;
        }
    }
    for(ValueSet* vs : defined_sets) {
        for(auto e : vs->elements) {
            dtable[e.first][e.second] = vs;
        }        
    }
}

void DeduceTable::freeSets() {
    for(ValueSet* vs : undefined_sets) {
        delete vs;
    }
    for(ValueSet* vs : defined_sets) {
        delete vs;
    }
}

void DeduceTable::print() const {
    for(int i=0; i<n; i++) {
        for (int j=0; j<n; j++) {
            const ValueSet* vs = dtable[i][j];
            if(i==cursor_row && j==cursor_col) 
                cout << "*";
            if(vs == NULL)
                cout << "NULL\t";
            else cout << vs->name << "\t";
        }
        cout << endl;
    }
}

void DeduceTable::log(string filename) const {
    cout << "a new matrix found! Logging it." << endl;
    Table t = Table(*this);
    t.check();
    ofstream ofs;
    ofs.open(filename, ios_base::app);
    ofs << t << endl;
    ofs.close();
}

int DeduceTable::listAllTables(string filename) {
    count = 0;
    while(1) {
        ValueSet* vs = dtable[cursor_row][cursor_col];
        // for debug
        // string temp;
        // cout << "ha~~~~~~~~~~~~~~~~~~~" << endl;
        // // print();
        // cout << "depth: " << call_depth << endl;
        // cout << "cursor: " << cursor_row << ", "
        //     << cursor_col << endl;
        // cout << "defined size: " << defined_sets.size() << endl;
        // cout << "undefined size: " << undefined_sets.size() << endl;
        // getchar();

        if(vs == NULL || vs->value==0)  {
            for(int i=vs->minvalue(); i<n; i++) {

                DeduceTable* newvs = make_copy();
                if(newvs->assign(i+1)) {
                    if(newvs->next()) {
                        newvs->log(filename);
                        count++;
                    } else {
                        count += newvs->listAllTables(filename);
                    }
                }
                newvs->freeSets();
                delete newvs;
            }
            return count;
        } else {
            // check monotonicity
            if(vs->value < cursor_row+1) {
                cout << "monotonicity violated, returning" << endl;
                return count;
            }
            // enforce commutativity 
            for(int i=0; i<cursor_col; i++) {
                if(!connect(readValue(cursor_row,cursor_col)-1, i, readValue(cursor_row,i)-1, cursor_col)) {
                    cout << "commutativity violated, returning" << endl;
                    return count;
                }
            }
        
            if(next()) {
                log(filename);
                count++;
                // cout << call_depth << " done, goes back" << endl;
                return count;
            } 
        }
    }
}

Table::Table(int nn) {
    n = nn;
    for(int i=0; i<n; i++) {
        table.push_back(vector<int>(n));
    }
}

Table::Table(const DeduceTable& dt) {
    n = dt.n;
    for(int i=0; i<n; i++) {
        table.push_back(vector<int>(n));
    }
    for(int i=0; i<n; i++) {
        for (int j=0; j<n; j++) {
            ValueSet* vs = dt.dtable[i][j];
            if(vs == NULL) {
                cerr << "Error: tabling unfinished deduceTable (NULL)" << endl;
                dt.print();
                exit(-1);
            }
            if(vs->value == 0) {
                cerr << "Error: tabling unfinished deduceTable (value 0)" << endl;
                dt.print();
                exit(-1);
            }
            else table[i][j] = vs->value;
        }
    }
}

void Table::print() {
    for(int i=0; i<n; i++) {
        for (int j=0; j<n; j++) {
            cout << table[i][j] << "\t";
        }
        cout << endl;
    }
}

void Table::check() {
    cout << "checking matrix:" << endl;
    print();
    if(    !checkAchievability()
        || !checkIdemPotence()
        || !checkCommutativity()
        || !checkMonotonicity()
    ) {
        cout << "axiom check fails" << endl;
        exit(-1);
    } else {
        cout << "OK!" << endl;
    }
}

bool Table::checkAchievability() {
    for(int i=0; i<n; i++) {
        if(table[0][i] != i+1) {
            cout << "achievability fails" << endl;
            cout << "i= " << i << endl;
            return false;
        }
    }
    return true;
}

bool Table::checkIdemPotence() {
    for(int i=0; i<n; i++) {
        for(int j=0; j<n; j++) {
            if(table[table[i][j]-1][j] != table[i][j]) {
                cout << "idempotence check fails" << endl;
                cout << "(i,k)= " << i << " " << j << endl;
                return false;
            }
        }
    }
    return true;
}

bool Table::checkCommutativity() {
    for(int i=0; i<n; i++) {
        for(int k=0; k<n; k++) {
            for(int l=0; l<n; l++) {
                if(table[table[i][k]-1][l]
                    !=
                    table[table[i][l]-1][k]
                ) {
                    cout << "commutativity check fails" << endl;
                    cout << "(i,k,l)= " << i << " "
                        << k << " " << l << endl;
                    return false;
                }
            }
        }
    }
    return true;
}

bool Table::checkMonotonicity() {
    for(int i=0; i<n; i++) {
        for(int j=0; j<n; j++) {
            if(table[i][j] < i+1) {
                cout << "monotonicity check fails" << endl;
                cout << "(i,k)= " << i << " " << j << endl;
                return false;
            }
        }
    }
    return true;
}

istream& operator>> (istream& ifs, Table& t) {
    for(int i=0; i<t.n; i++) {
        for (int j=0; j<t.n; j++) {
            ifs >> t.table[i][j];
        }
    }
    return ifs;
}

ostream& operator<< (ostream& ofs, Table& t) {
    for(int i=0; i<t.n; i++) {
        for (int j=0; j<t.n; j++) {
            ofs << t.table[i][j] << "\t";
        }
        ofs << endl;
    }
    return ofs;
}

bool operator== (Table& t1, Table& t2) {
    if(t1.n != t2.n) 
        return false;
    for(int i=0; i<t1.n; i++) {
        for(int j=0; j<t1.n; j++) {
            if(t1.table[i][j] != t2.table[i][j])
                return false;
        }
    }
    return true;
}

void StateMatrix::transpose() {
    // completeness check
    for(int i=0; i<dim; i++) {
        if((int) m[i].v.size()<dim) {
            cerr << "Error: transposing a partial matrix" << endl;
            exit(-1);
        }
    }

    for(int i=0; i<dim; i++) {
        for(int j=0; j<dim; j++) {
            int temp;
            temp = (m[i].v)[j];
            (m[i].v)[j] = (m[j].v)[i];
            (m[j].v)[i] = temp;
        }
    }
}

void StateMatrix::appendTo(string file)  {
    ofstream ofs;
    ofs.open(file, ios_base::app);
    for(int i=0; i<dim; i++) {
        for(int j=0; j<dim; j++) {
            ofs << (m[j].v)[i] << "\t";
        }
        ofs << endl;
    }
    ofs << endl;
    ofs.close();
}

void Table::diagonalize(string Pfile, string QTfile) {
    StateMatrix QT(n);
    // calculate QT, whose columns are all children
    for (int i=0; i<n; i++) {
        StateVector temp(n);
        for(int j=0; j<n; j++) {
            temp.v[table[j][i]-1] = 1;
        }
        QT.m[i] = temp;
    }
    QT.appendTo(QTfile);

    StateMatrix P(n);
    for(int k=0; k<n; k++) {
        StateVector temp(n);
        temp.v[k] = 1;
        for(int j=0; j<n; j++) {
            int jumpto = table[k][j]-1;
            if(jumpto == k)
                continue;
            StateVector temp2(n);
            temp2 = temp;
            for(int i=0; i<n; i++) {
                temp2.v[table[i][j]-1] -= temp.v[i];
            }
            temp = temp2;
        }
        P.m[k] = temp;
    }
    P.appendTo(Pfile);
}
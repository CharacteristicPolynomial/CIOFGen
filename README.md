# CIOFGen
A generator for all CIOFs (commutative and idempotent operation families) with configurable state size.

## Specifications
- Input: (int) n
- Output a file which contains all matrices M with entries from 1 to n, satisfying the following requirements: (format: M(row,col), in math convention, i.e. first index is 1)
    1. Achievability. M(1,i) = i for all i.
    2. Idempotence. M(M(i,k),k) = M(i,k) for all i, k.
    3. Commutativity. M(M(i,k),l) = M(M(i,l),k) for all i, k < l.
    4. Monotonicity. M(i,k) >= i.

### Examples
    "Counter" type CIOF: 
                1   2   3   4       
                2   2   3   4       
                3   3   3   4       
                4   4   4   4       
    
    "Bit-Group" type CIOF:
                1   2   3   4
                2   2   4   4
                3   4   3   4
                4   4   4   4

### P1. M is symmetric, i.e. M(i,k) = M(k,i) for all k,i.
Proof: M(i,k) = M(M(1,i),k) = M(M(1,k),i) = M(k,i) by S1 and S3.

### P2. M(k,k) = k for all k.
Proof: M(k,k) = M(M(1,k),k) = M(1,k) = k by S1 and S2.

### P3. If for any i,j,k, M(i,j) = k, then M(l,k) = M(M(l,i),j) for all l.
Proof: M(l,k) = M(k,l) = M(M(i,j),l) = M(M(i,l),j) = M(M(l,i),j) by P1 and S3.

Proposition 3 (P3) is useful in the sense that if we have M(i,j) = k, then the column k can be deduced from column i and column j.

## Algorithm
We use an algebraic style algorithm. Basically, we remain a family of disjoint sets. Entries that are in the same set has the same value. 

Before doing any operation to an entry, if it has a null pointer, then register a valueset in undefined_sets.

There are two basic operations, we ensure that before doing any operation, the entry is pointing to some valueset.
1. Set value to an entry
    - set the value of the pointing valueset. Assert if the valueset has already been defined a value.
2. Connect two entries
    - join the first valueset to the second valueset, redirect pointers to ensure invariance, and free the first valueset. If the two valuesets are both undefined, then the new set is undefined (return true); if exactly one of them is defined, follow the defined value (return false). If two sets are both defined return true if they matched, return false otherwise.

The algorithm goes by assigning values.
1. Init a dtable. (Ensure achievability, i.e., assign the first row)
2. Move the cursor until an undefined or NULL entry is found. If the cursor moves to the end of the matrix, which means a matrix is completely deduced, convert it to a normal table and log it to the file.
3. Create copies of dtable that assign the entry with values that respect the mononicity axiom. Each copy starts from step 2.

## Data Structures
    // remember throughout this project, we are using MATH convention
    // for values and CS convention for positions.

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
        Table();

        int n;
        vector<vector<int>> table;

        friend ifstream& operator>> (ifstream& ifs, Table& t);
        friend ofstream& operator<< (ofstream& ofs, Table& t);

        void print();
        void check();
        bool checkAchievability();
        bool checkIdemPotence();
        bool checkCommutativity();
        bool checkMonotonicity();
    };
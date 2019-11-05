# CIOFGen
A generator for all CIOFs (commutative and idempotent operation families) with configurable state size.

## Specifications
- Input: (int) n
- Output a file which contains all matrices M with entries from 1 to n, satisfying the following requirements: (format: M(row,col), in math convention, i.e. first index is 1)
    1. Achievability. M(1,i) = i for all i.
    2. Idempotence. M(M(i,k),k) = M(i,k) for all i, k.
    3. Commutativity. M(M(i,k),l) = M(M(i,l),k) for all i, k < l.
    4. Monotony. M(i,k) >= i.

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
We use an algebraic style algorithm. Basically, we remain a family of disjoint sets. Entries that are in the same set has the same value. By adding value for an entry, it
1. adds value to the set that is pointed by the entry;
2. causes some set to be joined as a result of S3;
3. causes some entry to be set as a result of S2.

## Data Structures
    // remember throughout this project, we are using MATH convention
    // for values and CS convention for positions.

    class ValueSet {
        public:
        ValueSet (int v, const vector<pair<int,int>>& e);
        string name; // a name for it (in order for print with undefined value)
        int value; // 0 if undefined, which acts as an unknown.
        // if value==0, then this is stored in undefined_sets
        // if value>0, then this is stored at defined_sets[value-1];
        vector<pair<int,int>> elements; // store the positions of the set elements
    };

    class DeduceTable {
        // algebraic table for deducing/generating
        public:
        int unknowns;
        int n;

        unordered_set<ValueSet*> undefined_sets; // undefined sets
        vector<ValueSet*> defined_sets; // value sets 1,2,...,n
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
        void freeSets(); // free all memory allocation (call it for destruction)
        
        void listAllTables(string filename); //

        string hashName(ValueSet* vs); // return a hashed name with length 3
        void print();

        // variables for performance inspection
        int call_depth; // the depth of its call stack
    }

    class Table {
        // value table for file I/O and property check
        public:
        Table(const DeduceTable& dt);
        Table();

        int n;
        vector<vector<int>> table;

        friend ifstream& operator>> (ifstream& ifs, Table& t);
        friend ofstream& operator<< (ofstream& ofs, Table& t);

        bool checkAchievability();
        bool checkIdemPotence();
        bool checkCommutativity();
    }

# CIOFGen
A generator for all CIOFs (commutative and idempotent operation families) with configurable state size.

## Executables
    $ make              # complie executables
    $ ./gen 5           # generate all 5*5 CIOF
    $ ./check 5 file    # check the file
    $ ./diag 5 file     # diagonalize

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
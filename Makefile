all: check gen diag

check: check.cpp table.cpp
	g++ -Wall check.cpp table.cpp -o check

gen: gen.cpp table.cpp
	g++ -Wall gen.cpp table.cpp -o gen

diag: diag.cpp table.cpp
	g++ -Wall diag.cpp table.cpp -o diag
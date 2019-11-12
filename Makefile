all: check gen diag checkDistinct

check: check.cpp table.cpp
	g++ -Wall check.cpp table.cpp -o check

checkDistinct: checkDistinct.cpp table.cpp
	g++ -Wall checkDistinct.cpp table.cpp -o checkDistinct

gen: gen.cpp table.cpp
	g++ -Wall gen.cpp table.cpp -o gen

diag: diag.cpp table.cpp
	g++ -Wall diag.cpp table.cpp -o diag
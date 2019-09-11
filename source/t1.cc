#include <iostream>
#include "mt.hh"

using namespace mtns;
using namespace std;

int main(void) {
    MovieTable *mt;
    bool  r;

    cout << "Instantiating VectorDbTable..." << endl;
    mt = new MovieTable();

    cout << "Invoking loadCSV(\"dummy\")..." << endl;
    r = mt->loadCSV("notscifi.csv");
    cout << "Expected return value: 0" << endl;
    cout << "Actual return value  : " << r << endl;

    cout << "Invoking loadCSV(\"default.csv\")..." << endl;
    r = mt->loadCSV("scifi.csv");
    cout << "Expected return value: 1" << endl;
    cout << "Actual return value  : " << r << endl;
    cout << "Expected : rows = 25" << endl;
    cout << "Actual   : rows = " << mt->rows() << endl;

    for(int i=0; i<25; i++) {
        if (i % 6 != 0) continue;
        cout << "Invoking show(" << i << ")..." << endl;
        r = mt->show(i);
        cout << "Expected return value: 1" << endl;
        cout << "Actual return value  : " << r << endl;
    }

    cout << "\nFreeing VectorDbTable.." << endl;
    delete mt;

    return 0;
}
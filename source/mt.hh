#ifndef _mt_hh_
#define _mt_hh_

#include <vector>
#include <cstring>
#include <iostream>

using namespace std;

namespace mtns {

    struct movie {
        unsigned short random;
        unsigned short year;
        char title[50];
        char director[30];
        char location[30];
    };

    class MovieTable {

    public:
        MovieTable();
        ~MovieTable();
        bool loadCSV(const char *infn);
        bool add(movie &movieRef);
        string show(unsigned long int row);
        string showAll();
        int rows();
        bool clear();
        movie *get(unsigned long int row);

    private:
        vector<movie> table;
    };
}

#endif
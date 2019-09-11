#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include "mt.hh"

using namespace mtns;
using namespace std;

MovieTable::MovieTable() = default;
MovieTable::~MovieTable() { table.clear(); }

int MovieTable::rows() {return table.size();}

bool MovieTable::add(movie &movieRef) {
    int previousSize = table.size();
    table.push_back(movieRef); // Add movie to the tail of the vector.
    return table.size() == previousSize + 1; // If movie was added successfully the rows should have increased by 1.
}

movie* MovieTable::get(unsigned long int row) {
    // as row is unsigned it cannot be negative.
    if (row >= rows()) { // Row is too big.
        cout << "invalid row" << endl;
        return nullptr;
    }

    else return &table[row]; // otherwise return the row.
}

string MovieTable::showAll() {
    stringstream output;
    for (int i=0; i<rows(); ++i) {
        output << show(i);
        if (i != rows()-1) output << "\n";
    }
    return output.str();
}

string MovieTable::show(unsigned long int row) {
    stringstream output;
    if (row >= rows()) { // Row is too big.
        cout << "invalid row" << endl;
        return output.str();
    }
    movie tableRow = table[row];
    output << "- " << tableRow.title << " " << tableRow.director << " " << tableRow.year << " " << tableRow.random << " " << tableRow.location;
    return output.str();
}

bool MovieTable::clear() {
    if (table == nullptr)
        return false;
    else {
        table.clear();
        return true;
    }
}

bool MovieTable::loadCSV(const char *infn) {
    ifstream ifs(infn); // Create an inputFileStream.
    if (!ifs.is_open()) { // File open failure. Abort.
        cout << "failed to open file: " << infn << endl;
        return false;
    }

    try { // try catch to collect any casting exceptions.
        while (!ifs.eof()) { // Keep going until we reach end of file.
            struct movie m = {};
            string token;
            // Movie Title
            getline(ifs, token, ',');
            strcpy(m.title, token.c_str());
            // Director
            getline(ifs, token, ',');
            strcpy(m.director, token.c_str()); // copy string char array to director within movie.
            // Movie Year
            getline(ifs, token, ',');
            m.year = (unsigned short) stoul(token); // casting string to unsigned long then trim to unsigned short.
            // Movie Random
            getline(ifs, token, ',');
            m.random = (unsigned short) stoul(token); // casting string to unsigned long
            // Location
            getline(ifs, token, '\n');
            strcpy(m.location, token.c_str());
            // Add movie to db
            add(m);
        }

        ifs.close(); // close the fileStream.

    } catch (...) { // cout below kinda explains it.
        cout << "File reading failed due to invalid formatting. (Perhaps there is an empty line?)" << endl;
        ifs.close(); // Attempt to close fileStream.
        return false;
    }
    return true;
}
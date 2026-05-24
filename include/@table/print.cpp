// print.cpp - utility to load a CSV as a table and print it using TableDB
#include "TableDB.h"
#include <iostream>
#include <filesystem>

static std::string derive_table_name(const std::string &path) {
    std::filesystem::path p(path);
    return p.stem().string();
}

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: print <file.csv>\n";
        return 1;
    }
    std::string path = argv[1];
    TableDB db;
    if (!db.load_table(path)) {
        std::cerr << "Failed to load: " << path << std::endl;
        return 2;
    }
    std::string name = derive_table_name(path);
    auto t = db.get_table(name);
    if (!t) {
        std::cerr << "Loaded but table not found: " << name << std::endl;
        return 3;
    }
    std::cout << "Table: " << t->name << "\n";
    std::cout << "Columns: ";
    for (auto &c : t->columns) std::cout << c << " ";
    std::cout << "\nRows:\n";
    for (size_t i = 0; i < t->rows.size(); ++i) {
        for (size_t j = 0; j < t->rows[i].size(); ++j) {
            std::cout << t->rows[i][j];
            if (j + 1 < t->rows[i].size()) std::cout << ", ";
        }
        std::cout << "\n";
    }
    return 0;
}

// table_cli.cpp - simple CLI to interact with TableDB
#include "TableDB.h"
#include <iostream>
#include <sstream>

static void print_help() {
    std::cout << "TableDB CLI\n";
    std::cout << "Commands:\n";
    std::cout << "  create <table> <col1> <col2> ...\n";
    std::cout << "  insert <table> <val1> <val2> ...\n";
    std::cout << "  select <table>\n";
    std::cout << "  list\n";
    std::cout << "  save <table> <file.csv>\n";
    std::cout << "  load <file.csv>\n";
    std::cout << "  help\n";
    std::cout << "  exit\n";
}

int main(int argc, char **argv) {
    TableDB db;
    std::cout << "TableDB CLI - type 'help' for commands\n";
    std::string line;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;
        if (line.empty()) continue;
        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;
        if (cmd == "help") { print_help(); continue; }
        if (cmd == "exit") break;
        if (cmd == "create") {
            std::string tname; iss >> tname;
            std::vector<std::string> cols; std::string c;
            while (iss >> c) cols.push_back(c);
            if (tname.empty() || cols.empty()) { std::cout << "Usage: create <table> <col1> <col2> ...\n"; continue; }
            if (db.create_table(tname, cols)) std::cout << "Created table " << tname << "\n";
            else std::cout << "Table already exists\n";
            continue;
        }
        if (cmd == "insert") {
            std::string tname; iss >> tname;
            std::vector<std::string> vals; std::string v;
            while (iss >> v) vals.push_back(v);
            if (db.insert_row(tname, vals)) std::cout << "Inserted row into " << tname << "\n";
            else std::cout << "Insert failed (table missing or wrong column count)\n";
            continue;
        }
        if (cmd == "select") {
            std::string tname; iss >> tname;
            auto t = db.get_table(tname);
            if (!t) { std::cout << "Table not found\n"; continue; }
            std::cout << "Columns: ";
            for (auto &c : t->columns) std::cout << c << " ";
            std::cout << "\n";
            for (size_t i = 0; i < t->rows.size(); ++i) {
                std::cout << i+1 << ": ";
                for (auto &cell : t->rows[i]) std::cout << cell << " ";
                std::cout << "\n";
            }
            continue;
        }
        if (cmd == "list") {
            auto names = db.list_tables();
            for (auto &n : names) std::cout << n << "\n";
            continue;
        }
        if (cmd == "save") {
            std::string tname, file; iss >> tname >> file;
            if (db.save_table(tname, file)) std::cout << "Saved " << tname << " to " << file << "\n";
            else std::cout << "Save failed\n";
            continue;
        }
        if (cmd == "load") {
            std::string file; iss >> file;
            if (db.load_table(file)) std::cout << "Loaded " << file << "\n";
            else std::cout << "Load failed\n";
            continue;
        }
        std::cout << "Unknown command: " << cmd << "\n";
    }
    return 0;
}

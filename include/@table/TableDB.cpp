// TableDB.cpp - implementation
#include "TableDB.h"
#include <fstream>
#include <sstream>
#include <algorithm>

bool TableDB::create_table(const std::string &name, const std::vector<std::string> &columns) {
    if (tables_.find(name) != tables_.end()) return false;
    Table t;
    t.name = name;
    t.columns = columns;
    tables_.emplace(name, std::move(t));
    return true;
}

bool TableDB::drop_table(const std::string &name) {
    return tables_.erase(name) > 0;
}

bool TableDB::insert_row(const std::string &table, const std::vector<std::string> &values) {
    auto it = tables_.find(table);
    if (it == tables_.end()) return false;
    Table &t = it->second;
    if (values.size() != t.columns.size()) return false;
    t.rows.emplace_back(values);
    return true;
}

std::optional<Table> TableDB::get_table(const std::string &name) const {
    auto it = tables_.find(name);
    if (it == tables_.end()) return std::nullopt;
    return it->second;
}

std::vector<std::string> TableDB::list_tables() const {
    std::vector<std::string> out;
    out.reserve(tables_.size());
    for (const auto &p : tables_) out.push_back(p.first);
    std::sort(out.begin(), out.end());
    return out;
}

static std::string escape_csv(const std::string &s) {
    if (s.find_first_of(",\"\n") == std::string::npos) return s;
    std::string out = "\"";
    for (char c : s) {
        if (c == '"') out += '"';
        out += c;
    }
    out += '"';
    return out;
}

bool TableDB::save_table(const std::string &name, const std::string &path) const {
    auto it = tables_.find(name);
    if (it == tables_.end()) return false;
    const Table &t = it->second;
    std::ofstream ofs(path, std::ios::binary);
    if (!ofs) return false;
    // write columns
    for (size_t i = 0; i < t.columns.size(); ++i) {
        if (i) ofs << ',';
        ofs << escape_csv(t.columns[i]);
    }
    ofs << "\n";
    for (const auto &row : t.rows) {
        for (size_t i = 0; i < row.size(); ++i) {
            if (i) ofs << ',';
            ofs << escape_csv(row[i]);
        }
        ofs << "\n";
    }
    return true;
}

static std::vector<std::string> split_csv_line(const std::string &line) {
    std::vector<std::string> out;
    std::istringstream ss(line);
    std::string cell;
    bool inquote = false;
    std::string cur;
    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (!inquote) {
            if (c == ',') {
                out.push_back(cur);
                cur.clear();
            } else if (c == '"') {
                inquote = true;
            } else {
                cur += c;
            }
        } else {
            if (c == '"') {
                if (i + 1 < line.size() && line[i+1] == '"') {
                    cur += '"';
                    ++i;
                } else {
                    inquote = false;
                }
            } else {
                cur += c;
            }
        }
    }
    out.push_back(cur);
    return out;
}

bool TableDB::load_table(const std::string &path) {
    std::ifstream ifs(path, std::ios::binary);
    if (!ifs) return false;
    std::string header;
    if (!std::getline(ifs, header)) return false;
    auto columns = split_csv_line(header);
    // derive table name from filename
    std::string name = path;
    // naive: strip path separators
    auto pos = name.find_last_of("\\/");
    if (pos != std::string::npos) name = name.substr(pos + 1);
    // strip extension
    auto dot = name.find_last_of('.');
    if (dot != std::string::npos) name = name.substr(0, dot);

    Table t;
    t.name = name;
    t.columns = columns;
    std::string line;
    while (std::getline(ifs, line)) {
        if (line.empty()) continue;
        t.rows.push_back(split_csv_line(line));
    }
    tables_[t.name] = std::move(t);
    return true;
}

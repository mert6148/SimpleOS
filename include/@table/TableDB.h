// TableDB.h - Minimal in-memory table database for SimpleOS
#ifndef SIMPLEOS_TABLEDB_H
#define SIMPLEOS_TABLEDB_H

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>

struct Table {
    std::string name;
    std::vector<std::string> columns;
    std::vector<std::vector<std::string>> rows;
};

class TableDB {
public:
    TableDB() = default;
    ~TableDB() = default;

    bool create_table(const std::string &name, const std::vector<std::string> &columns);
    bool drop_table(const std::string &name);
    bool insert_row(const std::string &table, const std::vector<std::string> &values);
    std::optional<Table> get_table(const std::string &name) const;
    std::vector<std::string> list_tables() const;

    // Persistence
    bool save_table(const std::string &name, const std::string &path) const;
    bool load_table(const std::string &path);

private:
    std::unordered_map<std::string, Table> tables_;
};

#endif // SIMPLEOS_TABLEDB_H

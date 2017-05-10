
#ifndef LOAD_H
#define LOAD_H

//Used to replace apostraphes in strings with ''
void replaceAll(std::string& str, const std::string& from, const std::string& to);

//creates tables in the databse
void creatTable(const char* createCommand, std::string tableName, int cols, sqlite3 *db, const char* filePath);

//opens database, and perpares meta data for creating tables
int main(int argc, char* argv[]);

#endif
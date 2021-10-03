#include <iostream>
#include "Table/Table.h"

using namespace std;

int main() {
    string table_name;

    while (cin >> table_name && table_name != "exit") {
        string command;
        cin >> command;

        if (command == "create") {
            create_table(table_name, cin);
            continue;
        }

        Table table(table_name);
        if (command == "select")        table.select_rows(cout, cin);
        else if (command == "insert")   table.insert_row(cin);
        else if (command == "update")   table.update_rows(cin);
        else if (command == "delete")   table.delete_rows(cin);
        else                            cout << "Unrecognized command\n";
    }
}
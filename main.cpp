#include <iostream>
#include "Table/Table.h"

using namespace std;

int main() {
    create_table(std::cin);
    Table t("table");

    t.print();
}

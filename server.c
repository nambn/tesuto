#include <stdio.h>
#include "common.h"
#include <sqlite3.h>

main(int argc, char const *argv[])
{
    sqlite3 *db;

    if (sqlite3_open("tesuto.db", &db))
    {
        perror("Can't open database\n");
        return 0;
    }
    else
        printf("Open database successfully\n");
    return 0;
}

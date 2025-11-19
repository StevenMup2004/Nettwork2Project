#ifndef DB_H
#define DB_H

#include <mysql/mysql.h>

MYSQL* connect_db();
void disconnect_db(MYSQL *conn);

#endif 
#include "user.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int fetch_users(User **users, int *count){
    MYSQL *conn = connect_db();
    if (conn == NULL) {
        return 1;
    }

    if (mysql_query(conn, "SELECT * FROM users")) {
        fprintf(stderr, "SELECT failed. Error: %s\n", mysql_error(conn));
        disconnect_db(conn);
        return 2;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL) {
        fprintf(stderr, "mysql_store_result() failed. Error: %s\n", mysql_error(conn));
        disconnect_db(conn);
        return 3;
    }

    int num_rows = mysql_num_rows(res);
    *count = num_rows;
    *users = malloc(sizeof(User) * num_rows);

    for (int i = 0; i < num_rows; i++) {
        MYSQL_ROW row = mysql_fetch_row(res);
        (*users)[i].user_id = atoi(row[0]);
        strncpy((*users)[i].name, row[1], sizeof((*users)[i].name) - 1);
        strncpy((*users)[i].phone, row[2], sizeof((*users)[i].phone) - 1);
        strncpy((*users)[i].email, row[3], sizeof((*users)[i].email) - 1);
        strncpy((*users)[i].password, row[4], sizeof((*users)[i].password) - 1);
    }

    mysql_free_result(res);
    disconnect_db(conn);

    return 0;

}

int add_user(const User *user) {
    MYSQL *conn = connect_db();
    if (conn == NULL) {
        fprintf(stderr, "Failed to connect to database.\n");
        return 1;
    }

    const char *query = "INSERT INTO users (name, phone, email, password) VALUES (?, ?, ?, ?)";
    MYSQL_STMT *stmt = mysql_stmt_init(conn);
    if (stmt == NULL) {
        fprintf(stderr, "mysql_stmt_init() failed. Error: %s\n", mysql_error(conn));
        disconnect_db(conn);
        return 2;
    }

    if (mysql_stmt_prepare(stmt, query, strlen(query)) != 0) {
        fprintf(stderr, "mysql_stmt_prepare() failed. Error: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        disconnect_db(conn);
        return 3;
    }

    MYSQL_BIND bind[4];
    memset(bind, 0, sizeof(bind));

    bind[0].buffer_type = MYSQL_TYPE_STRING;
    bind[0].buffer = (char *)user->name;
    bind[0].buffer_length = strlen(user->name);

    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].buffer = (char *)user->phone;
    bind[1].buffer_length = strlen(user->phone);

    bind[2].buffer_type = MYSQL_TYPE_STRING;
    bind[2].buffer = (char *)user->email;
    bind[2].buffer_length = strlen(user->email);

    bind[3].buffer_type = MYSQL_TYPE_STRING;
    bind[3].buffer = (char *)user->password;
    bind[3].buffer_length = strlen(user->password);

    if (mysql_stmt_bind_param(stmt, bind) != 0) {
        fprintf(stderr, "mysql_stmt_bind_param() failed. Error: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        disconnect_db(conn);
        return 4;
    }

    if (mysql_stmt_execute(stmt) != 0) {
        fprintf(stderr, "mysql_stmt_execute() failed. Error: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        disconnect_db(conn);
        return 5;
    }

    mysql_stmt_close(stmt);
    disconnect_db(conn);

    printf("User added successfully.\n");
    return 0;
}
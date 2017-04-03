//
// Created by li su on 02/04/2017.
//
#include <stdio.h>
#include <mysql.h>
#include "string.h"

int test_text_query(MYSQL *connection, char *sql);

int test_prepare_query(MYSQL *connection, char *sql);

int main(void) {


    mysql_library_init;

    MYSQL *connection = mysql_init(NULL);
    if (connection == NULL) {
        fprintf(stderr, "Failed to initialize: %s\n", mysql_error(connection));
        return 1;
    }

    if (!mysql_real_connect(connection, "0.0.0.0", "root", "", "test", 3306, NULL, CLIENT_FOUND_ROWS)) {
        fprintf(stderr, "Failed to connect to server: %s\n", mysql_error(connection));
        return 1;
    }

    printf("MySQL server connected~\n");

//    if (test_text_query(connection, "insert into test_table (`key`, `value`) values (7, 3) on duplicate key update `value` = values(`value`)")) {
//        return 1;
//    }

    if (test_text_query(connection, "delete from test_table")) {
        return 1;
    }

    my_ulonglong affectedRow = mysql_affected_rows(connection);

    printf("affected_rows: %llu", affectedRow);

    mysql_close(connection);

}

int test_prepare_query(MYSQL *connection, char *sql) {

    MYSQL_STMT *stmt = mysql_stmt_init(connection);
    if (stmt == NULL) {
        fprintf(stderr, "Init stmt failure: %s\n", mysql_error(connection));
        return 1;
    }

    if (mysql_stmt_prepare(stmt, sql, strlen(sql))) {
        fprintf(stderr, "Prepare stmt failure: %s\n", mysql_stmt_error(stmt));
        return 1;
    }

    MYSQL_BIND bind[1];
    memset(bind, "", sizeof(bind));

    long x = 1;

    bind[0].buffer_type = MYSQL_TYPE_LONG;
    bind[0].buffer = (char *) &x;
    bind[0].is_null = 0;
    bind[0].length = 0;

    if (mysql_stmt_bind_param(stmt, bind)) {
        fprintf(stderr, "Bind stmt param failure: %s\n", mysql_stmt_error(stmt));
        return 1;
    }

    if (mysql_stmt_execute(stmt)) {
        fprintf(stderr, "Execute stmt error: %s\n", mysql_stmt_error(stmt));
        return 1;
    }

    memset(bind, "", 1);

    int result_value;
    unsigned long length_value;
    my_bool error_value;
    my_bool is_null_value;

    bind[0].buffer_type = MYSQL_TYPE_INT24;
    bind[0].buffer = (char *) &result_value;
    bind[0].length = &length_value;
    bind[0].is_null = &is_null_value;
    bind[0].error = &error_value;

    if (mysql_stmt_bind_result(stmt, bind)) {
        fprintf(stderr, "bind result failure: %s", mysql_stmt_error(stmt));
        return 1;
    }

    if (mysql_stmt_store_result(stmt)) {
        fprintf(stderr, "store result failure: %s", mysql_stmt_error(stmt));
        return 1;
    }

    while (!mysql_stmt_fetch(stmt)) {
        if (is_null_value) {
            fprintf(stdout, " NULL\n");
        } else {
            fprintf(stdout, " %li(%ld)\n", result_value, length_value);
        }
        fprintf(stdout, "\n");
    }


    mysql_stmt_close(stmt);

    return 0;

}

int test_text_query(MYSQL *connection, char *sql) {
    if (mysql_query(connection, sql)) {
        fprintf(stderr, "Execute query failure: %s\n", mysql_error(connection));
    }

    MYSQL_RES *result = mysql_use_result(connection);
    if (result == NULL) {
        printf("No result.\n");
        return 0;
    }

    MYSQL_ROW row;
    unsigned int num_fields = mysql_num_fields(result);
    while ((row = mysql_fetch_row(result))) {
        unsigned long *field_lengths = mysql_fetch_lengths(result);
        if (field_lengths == NULL) {
            fprintf(stderr, "Failed to get field lengths: %s\n", mysql_error(connection));
            return 1;
        }
        for (int i = 0; i < num_fields; i++) {
            printf("[%.*s] ", (int) field_lengths[i], row[i] ? row[i] : "NULL");
        }
        printf("\n");
    }
    mysql_free_result(result);
    return 0;
}


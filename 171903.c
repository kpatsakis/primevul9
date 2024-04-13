MONGO_EXPORT void mongo_cmd_reset_error( mongo *conn, const char *db ) {
    mongo_simple_int_command( conn, db, "reseterror", 1, NULL );
}
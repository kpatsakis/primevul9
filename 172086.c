MONGO_EXPORT int mongo_cmd_drop_db( mongo *conn, const char *db ) {
    return mongo_simple_int_command( conn, db, "dropDatabase", 1, NULL );
}
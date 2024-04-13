MONGO_EXPORT int mongo_cmd_get_last_error( mongo *conn, const char *db, bson *out ) {
    return mongo_cmd_get_error_helper( conn, db, out, "getlasterror" );
}
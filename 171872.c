MONGO_EXPORT int mongo_cmd_drop_collection( mongo *conn, const char *db, const char *collection, bson *out ) {
    return mongo_simple_str_command( conn, db, "drop", collection, out );
}
MONGO_EXPORT void mongo_dispose(mongo* conn) {
    bson_free(conn);
}
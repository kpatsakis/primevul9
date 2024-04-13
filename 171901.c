MONGO_EXPORT void mongo_cursor_dispose(mongo_cursor* cursor) {
    bson_free(cursor);
}
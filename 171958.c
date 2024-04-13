MONGO_EXPORT void mongo_cursor_set_limit( mongo_cursor *cursor, int limit ) {
    cursor->limit = limit;
}
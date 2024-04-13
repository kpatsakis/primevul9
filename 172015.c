MONGO_EXPORT void mongo_cursor_set_query( mongo_cursor *cursor, const bson *query ) {
    cursor->query = query;
}
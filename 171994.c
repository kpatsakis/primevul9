MONGO_EXPORT void mongo_cursor_set_skip( mongo_cursor *cursor, int skip ) {
    cursor->skip = skip;
}
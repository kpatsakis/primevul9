MONGO_EXPORT void mongo_cursor_set_options( mongo_cursor *cursor, int options ) {
    cursor->options = options;
}
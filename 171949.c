MONGO_EXPORT void mongo_cursor_set_fields( mongo_cursor *cursor, const bson *fields ) {
    cursor->fields = fields;
}
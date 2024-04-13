MONGO_EXPORT const char *mongo_cursor_data( mongo_cursor *cursor ) {
    return cursor->current.data;
}
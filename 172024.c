MONGO_EXPORT const bson *mongo_cursor_bson( mongo_cursor *cursor ) {
    return (const bson *)&(cursor->current);
}
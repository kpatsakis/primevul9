MONGO_EXPORT const char *bson_iterator_key( const bson_iterator *i ) {
    return i->cur + 1;
}
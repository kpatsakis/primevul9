MONGO_EXPORT const char *bson_iterator_value( const bson_iterator *i ) {
    const char *t = i->cur + 1;
    t += strlen( t ) + 1;
    return t;
}
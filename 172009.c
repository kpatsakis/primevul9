MONGO_EXPORT const char *bson_iterator_regex_opts( const bson_iterator *i ) {
    const char *p = bson_iterator_value( i );
    return p + strlen( p ) + 1;

}
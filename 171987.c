MONGO_EXPORT const char *bson_iterator_string( const bson_iterator *i ) {
    switch ( bson_iterator_type( i ) ) {
    case BSON_STRING:
    case BSON_SYMBOL:
        return bson_iterator_value( i ) + 4;
    default:
        return "";
    }
}
MONGO_EXPORT const char *bson_iterator_code( const bson_iterator *i ) {
    switch ( bson_iterator_type( i ) ) {
    case BSON_STRING:
    case BSON_CODE:
        return bson_iterator_value( i ) + 4;
    case BSON_CODEWSCOPE:
        return bson_iterator_value( i ) + 8;
    default:
        return NULL;
    }
}
MONGO_EXPORT bson_bool_t bson_iterator_bool( const bson_iterator *i ) {
    switch ( bson_iterator_type( i ) ) {
    case BSON_BOOL:
        return bson_iterator_bool_raw( i );
    case BSON_INT:
        return bson_iterator_int_raw( i ) != 0;
    case BSON_LONG:
        return bson_iterator_long_raw( i ) != 0;
    case BSON_DOUBLE:
        return bson_iterator_double_raw( i ) != 0;
    case BSON_EOO:
    case BSON_NULL:
        return 0;
    default:
        return 1;
    }
}
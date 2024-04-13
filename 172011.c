MONGO_EXPORT const char *bson_iterator_bin_data( const bson_iterator *i ) {
    return ( bson_iterator_bin_type( i ) == BSON_BIN_BINARY_OLD )
           ? bson_iterator_value( i ) + 9
           : bson_iterator_value( i ) + 5;
}
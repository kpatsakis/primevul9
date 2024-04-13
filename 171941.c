MONGO_EXPORT int bson_iterator_bin_len( const bson_iterator *i ) {
    return ( bson_iterator_bin_type( i ) == BSON_BIN_BINARY_OLD )
           ? bson_iterator_int_raw( i ) - 4
           : bson_iterator_int_raw( i );
}
int64_t bson_iterator_long_raw( const bson_iterator *i ) {
    int64_t out;
    bson_little_endian64( &out, bson_iterator_value( i ) );
    return out;
}
int bson_iterator_int_raw( const bson_iterator *i ) {
    int out;
    bson_little_endian32( &out, bson_iterator_value( i ) );
    return out;
}
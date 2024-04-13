double bson_iterator_double_raw( const bson_iterator *i ) {
    double out;
    bson_little_endian64( &out, bson_iterator_value( i ) );
    return out;
}
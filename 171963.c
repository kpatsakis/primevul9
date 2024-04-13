MONGO_EXPORT int bson_iterator_timestamp_increment( const bson_iterator *i ) {
    int increment;
    bson_little_endian32( &increment, bson_iterator_value( i ) );
    return increment;
}
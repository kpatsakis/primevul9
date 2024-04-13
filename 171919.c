MONGO_EXPORT int bson_iterator_timestamp_time( const bson_iterator *i ) {
    int time;
    bson_little_endian32( &time, bson_iterator_value( i ) + 4 );
    return time;
}
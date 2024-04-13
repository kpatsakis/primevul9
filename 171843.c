MONGO_EXPORT bson_timestamp_t bson_iterator_timestamp( const bson_iterator *i ) {
    bson_timestamp_t ts;
    bson_little_endian32( &( ts.i ), bson_iterator_value( i ) );
    bson_little_endian32( &( ts.t ), bson_iterator_value( i ) + 4 );
    return ts;
}
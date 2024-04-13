MONGO_EXPORT bson_date_t bson_iterator_date( const bson_iterator *i ) {
    return bson_iterator_long_raw( i );
}
MONGO_EXPORT time_t bson_iterator_time_t( const bson_iterator *i ) {
    return bson_iterator_date( i ) / 1000;
}
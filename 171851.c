bson_bool_t bson_iterator_bool_raw( const bson_iterator *i ) {
    return bson_iterator_value( i )[0];
}
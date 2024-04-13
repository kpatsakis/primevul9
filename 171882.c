MONGO_EXPORT char bson_iterator_bin_type( const bson_iterator *i ) {
    return bson_iterator_value( i )[4];
}
MONGO_EXPORT bson_type bson_iterator_type( const bson_iterator *i ) {
    return ( bson_type )i->cur[0];
}
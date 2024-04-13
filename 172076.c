MONGO_EXPORT bson_bool_t bson_iterator_more( const bson_iterator *i ) {
    return *( i->cur );
}
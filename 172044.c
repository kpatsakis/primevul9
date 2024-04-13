MONGO_EXPORT bson_iterator* bson_iterator_create( void ) {
    return ( bson_iterator* )malloc( sizeof( bson_iterator ) );
}
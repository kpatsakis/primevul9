MONGO_EXPORT bson_oid_t *bson_iterator_oid( const bson_iterator *i ) {
    return ( bson_oid_t * )bson_iterator_value( i );
}
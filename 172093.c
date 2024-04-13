MONGO_EXPORT void bson_iterator_subiterator( const bson_iterator *i, bson_iterator *sub ) {
    bson_iterator_from_buffer( sub, bson_iterator_value( i ) );
}
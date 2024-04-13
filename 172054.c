MONGO_EXPORT void bson_iterator_subobject( const bson_iterator *i, bson *sub ) {
    bson_init_data( sub, ( char * )bson_iterator_value( i ) );
    _bson_reset( sub );
    sub->finished = 1;
}
MONGO_EXPORT int bson_size( const bson *b ) {
    int i;
    if ( ! b || ! b->data )
        return 0;
    bson_little_endian32( &i, b->data );
    return i;
}
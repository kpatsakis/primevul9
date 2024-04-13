MONGO_EXPORT void bson_init( bson *b ) {
    _bson_init_size( b, initialBufferSize );
}
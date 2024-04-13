MONGO_EXPORT void bson_free( void *ptr ) {
    bson_free_func( ptr );
}
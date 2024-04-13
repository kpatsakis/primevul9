MONGO_EXPORT int bson_append_start_array( bson *b, const char *name ) {
    if ( bson_append_estart( b, BSON_ARRAY, name, 5 ) == BSON_ERROR ) return BSON_ERROR;
    b->stack[ b->stackPos++ ] = b->cur - b->data;
    bson_append32( b , &zero );
    return BSON_OK;
}
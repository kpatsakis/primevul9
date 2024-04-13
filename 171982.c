MONGO_EXPORT int bson_append_bson( bson *b, const char *name, const bson *bson ) {
    if ( !bson ) return BSON_ERROR;
    if ( bson_append_estart( b, BSON_OBJECT, name, bson_size( bson ) ) == BSON_ERROR )
        return BSON_ERROR;
    bson_append( b , bson->data , bson_size( bson ) );
    return BSON_OK;
}
MONGO_EXPORT int bson_append_null( bson *b, const char *name ) {
    if ( bson_append_estart( b , BSON_NULL, name, 0 ) == BSON_ERROR )
        return BSON_ERROR;
    return BSON_OK;
}
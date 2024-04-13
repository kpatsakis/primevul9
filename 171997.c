MONGO_EXPORT int bson_append_undefined( bson *b, const char *name ) {
    if ( bson_append_estart( b, BSON_UNDEFINED, name, 0 ) == BSON_ERROR )
        return BSON_ERROR;
    return BSON_OK;
}
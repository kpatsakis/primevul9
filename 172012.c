MONGO_EXPORT int bson_append_bool( bson *b, const char *name, const bson_bool_t i ) {
    if ( bson_append_estart( b, BSON_BOOL, name, 1 ) == BSON_ERROR )
        return BSON_ERROR;
    bson_append_byte( b , i != 0 );
    return BSON_OK;
}
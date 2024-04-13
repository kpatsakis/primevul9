MONGO_EXPORT int bson_append_long( bson *b, const char *name, const int64_t i ) {
    if ( bson_append_estart( b , BSON_LONG, name, 8 ) == BSON_ERROR )
        return BSON_ERROR;
    bson_append64( b , &i );
    return BSON_OK;
}
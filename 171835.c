MONGO_EXPORT int bson_append_int( bson *b, const char *name, const int i ) {
    if ( bson_append_estart( b, BSON_INT, name, 4 ) == BSON_ERROR )
        return BSON_ERROR;
    bson_append32( b , &i );
    return BSON_OK;
}
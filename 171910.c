MONGO_EXPORT int bson_append_timestamp2( bson *b, const char *name, int time, int increment ) {
    if ( bson_append_estart( b, BSON_TIMESTAMP, name, 8 ) == BSON_ERROR ) return BSON_ERROR;

    bson_append32( b , &increment );
    bson_append32( b , &time );
    return BSON_OK;
}
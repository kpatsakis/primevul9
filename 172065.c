MONGO_EXPORT int bson_append_timestamp( bson *b, const char *name, bson_timestamp_t *ts ) {
    if ( bson_append_estart( b, BSON_TIMESTAMP, name, 8 ) == BSON_ERROR ) return BSON_ERROR;

    bson_append32( b , &( ts->i ) );
    bson_append32( b , &( ts->t ) );

    return BSON_OK;
}
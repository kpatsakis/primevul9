MONGO_EXPORT int bson_append_date( bson *b, const char *name, bson_date_t millis ) {
    if ( bson_append_estart( b, BSON_DATE, name, 8 ) == BSON_ERROR ) return BSON_ERROR;
    bson_append64( b , &millis );
    return BSON_OK;
}
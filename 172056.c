MONGO_EXPORT int bson_append_double( bson *b, const char *name, const double d ) {
    if ( bson_append_estart( b, BSON_DOUBLE, name, 8 ) == BSON_ERROR )
        return BSON_ERROR;
    bson_append64( b , &d );
    return BSON_OK;
}
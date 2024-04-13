MONGO_EXPORT int bson_append_oid( bson *b, const char *name, const bson_oid_t *oid ) {
    if ( bson_append_estart( b, BSON_OID, name, 12 ) == BSON_ERROR )
        return BSON_ERROR;
    bson_append( b , oid , 12 );
    return BSON_OK;
}
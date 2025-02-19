MONGO_EXPORT void bson_oid_from_string( bson_oid_t *oid, const char *str ) {
    int i;
    for ( i=0; i<12; i++ ) {
        oid->bytes[i] = ( hexbyte( str[2*i] ) << 4 ) | hexbyte( str[2*i + 1] );
    }
}
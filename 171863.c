MONGO_EXPORT void bson_oid_to_string( const bson_oid_t *oid, char *str ) {
    static const char hex[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
    int i;
    for ( i=0; i<12; i++ ) {
        str[2*i]     = hex[( oid->bytes[i] & 0xf0 ) >> 4];
        str[2*i + 1] = hex[ oid->bytes[i] & 0x0f      ];
    }
    str[24] = '\0';
}
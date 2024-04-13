static bson *chunk_new( bson_oid_t id, int chunkNumber,
                        const char *data, int len ) {
    bson *b = bson_malloc( sizeof( bson ) );

    bson_init( b );
    bson_append_oid( b, "files_id", &id );
    bson_append_int( b, "n", chunkNumber );
    bson_append_binary( b, "data", BSON_BIN_BINARY, data, len );
    bson_finish( b );
    return  b;
}
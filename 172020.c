static void chunk_free( bson *oChunk ) {
    bson_destroy( oChunk );
    bson_free( oChunk );
}
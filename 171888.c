MONGO_EXPORT int gridfs_store_buffer( gridfs *gfs, const char *data,
                                      gridfs_offset length, const char *remotename,
                                      const char *contenttype ) {

    char const *end = data + length;
    const char *data_ptr = data;
    bson_oid_t id;
    int chunkNumber = 0;
    int chunkLen;
    bson *oChunk;

    /* Large files Assertion */
    /* assert( length <= 0xffffffff ); */

    /* Generate and append an oid*/
    bson_oid_gen( &id );

    /* Insert the file's data chunk by chunk */
    while ( data_ptr < end ) {
        chunkLen = DEFAULT_CHUNK_SIZE < ( unsigned int )( end - data_ptr ) ?
                   DEFAULT_CHUNK_SIZE : ( unsigned int )( end - data_ptr );
        oChunk = chunk_new( id, chunkNumber, data_ptr, chunkLen );
        mongo_insert( gfs->client, gfs->chunks_ns, oChunk, NULL );
        chunk_free( oChunk );
        chunkNumber++;
        data_ptr += chunkLen;
    }

    /* Inserts file's metadata */
    return gridfs_insert_file( gfs, remotename, id, length, contenttype );
}
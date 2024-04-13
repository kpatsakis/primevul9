int gridfs_store_file( gridfs *gfs, const char *filename,
                       const char *remotename, const char *contenttype ) {

    char buffer[DEFAULT_CHUNK_SIZE];
    FILE *fd;
    bson_oid_t id;
    int chunkNumber = 0;
    gridfs_offset length = 0;
    gridfs_offset chunkLen = 0;
    bson *oChunk;

    /* Open the file and the correct stream */
    if ( strcmp( filename, "-" ) == 0 ) fd = stdin;
    else {
        fd = fopen( filename, "rb" );
        if (fd == NULL)
            return MONGO_ERROR;
    }

    /* Generate and append an oid*/
    bson_oid_gen( &id );

    /* Insert the file chunk by chunk */
    chunkLen = fread( buffer, 1, DEFAULT_CHUNK_SIZE, fd );
    do {
        oChunk = chunk_new( id, chunkNumber, buffer, chunkLen );
        mongo_insert( gfs->client, gfs->chunks_ns, oChunk, NULL );
        chunk_free( oChunk );
        length += chunkLen;
        chunkNumber++;
        chunkLen = fread( buffer, 1, DEFAULT_CHUNK_SIZE, fd );
    }
    while ( chunkLen != 0 );

    /* Close the file stream */
    if ( fd != stdin ) fclose( fd );

    /* Large files Assertion */
    /* assert(length <= 0xffffffff); */

    /* Optional Remote Name */
    if ( remotename == NULL || *remotename == '\0' ) {
        remotename = filename;
    }

    /* Inserts file's metadata */
    return gridfs_insert_file( gfs, remotename, id, length, contenttype );
}
int gridfs_init( mongo *client, const char *dbname, const char *prefix,
                 gridfs *gfs ) {

    int options;
    bson b;
    bson_bool_t success;

    gfs->client = client;

    /* Allocate space to own the dbname */
    gfs->dbname = ( const char * )bson_malloc( strlen( dbname )+1 );
    strcpy( ( char * )gfs->dbname, dbname );

    /* Allocate space to own the prefix */
    if ( prefix == NULL ) prefix = "fs";
    gfs->prefix = ( const char * )bson_malloc( strlen( prefix )+1 );
    strcpy( ( char * )gfs->prefix, prefix );

    /* Allocate space to own files_ns */
    gfs->files_ns =
        ( const char * ) bson_malloc ( strlen( prefix )+strlen( dbname )+strlen( ".files" )+2 );
    strcpy( ( char * )gfs->files_ns, dbname );
    strcat( ( char * )gfs->files_ns, "." );
    strcat( ( char * )gfs->files_ns, prefix );
    strcat( ( char * )gfs->files_ns, ".files" );

    /* Allocate space to own chunks_ns */
    gfs->chunks_ns = ( const char * ) bson_malloc( strlen( prefix ) + strlen( dbname )
                     + strlen( ".chunks" ) + 2 );
    strcpy( ( char * )gfs->chunks_ns, dbname );
    strcat( ( char * )gfs->chunks_ns, "." );
    strcat( ( char * )gfs->chunks_ns, prefix );
    strcat( ( char * )gfs->chunks_ns, ".chunks" );

    bson_init( &b );
    bson_append_int( &b, "filename", 1 );
    bson_finish( &b );
    options = 0;
    success = ( mongo_create_index( gfs->client, gfs->files_ns, &b, options, NULL ) == MONGO_OK );
    bson_destroy( &b );
    if ( !success ) {
        bson_free( ( char * )gfs->dbname );
        bson_free( ( char * )gfs->prefix );
        bson_free( ( char * )gfs->files_ns );
        bson_free( ( char * )gfs->chunks_ns );
        return MONGO_ERROR;
    }

    bson_init( &b );
    bson_append_int( &b, "files_id", 1 );
    bson_append_int( &b, "n", 1 );
    bson_finish( &b );
    options = MONGO_INDEX_UNIQUE;
    success = ( mongo_create_index( gfs->client, gfs->chunks_ns, &b, options, NULL ) == MONGO_OK );
    bson_destroy( &b );
    if ( !success ) {
        bson_free( ( char * )gfs->dbname );
        bson_free( ( char * )gfs->prefix );
        bson_free( ( char * )gfs->files_ns );
        bson_free( ( char * )gfs->chunks_ns );
        return MONGO_ERROR;
    }

    return MONGO_OK;
}
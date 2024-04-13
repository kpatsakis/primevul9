MONGO_EXPORT void gridfs_remove_filename( gridfs *gfs, const char *filename ) {
    bson query;
    mongo_cursor *files;
    bson file;
    bson_iterator it;
    bson_oid_t id;
    bson b;

    bson_init( &query );
    bson_append_string( &query, "filename", filename );
    bson_finish( &query );
    files = mongo_find( gfs->client, gfs->files_ns, &query, NULL, 0, 0, 0 );
    bson_destroy( &query );

    /* Remove each file and it's chunks from files named filename */
    while ( mongo_cursor_next( files ) == MONGO_OK ) {
        file = files->current;
        bson_find( &it, &file, "_id" );
        id = *bson_iterator_oid( &it );

        /* Remove the file with the specified id */
        bson_init( &b );
        bson_append_oid( &b, "_id", &id );
        bson_finish( &b );
        mongo_remove( gfs->client, gfs->files_ns, &b, NULL );
        bson_destroy( &b );

        /* Remove all chunks from the file with the specified id */
        bson_init( &b );
        bson_append_oid( &b, "files_id", &id );
        bson_finish( &b );
        mongo_remove( gfs->client, gfs->chunks_ns, &b, NULL );
        bson_destroy( &b );
    }

    mongo_cursor_destroy( files );
}
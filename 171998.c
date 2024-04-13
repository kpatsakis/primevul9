static int gridfs_insert_file( gridfs *gfs, const char *name,
                               const bson_oid_t id, gridfs_offset length,
                               const char *contenttype ) {
    bson command;
    bson ret;
    bson res;
    bson_iterator it;
    int result;
    int64_t d;

    /* Check run md5 */
    bson_init( &command );
    bson_append_oid( &command, "filemd5", &id );
    bson_append_string( &command, "root", gfs->prefix );
    bson_finish( &command );
    result = mongo_run_command( gfs->client, gfs->dbname, &command, &res );
    bson_destroy( &command );
    if (result != MONGO_OK)
        return result;

    /* Create and insert BSON for file metadata */
    bson_init( &ret );
    bson_append_oid( &ret, "_id", &id );
    if ( name != NULL && *name != '\0' ) {
        bson_append_string( &ret, "filename", name );
    }
    bson_append_long( &ret, "length", length );
    bson_append_int( &ret, "chunkSize", DEFAULT_CHUNK_SIZE );
    d = ( bson_date_t )1000*time( NULL );
    bson_append_date( &ret, "uploadDate", d);
    bson_find( &it, &res, "md5" );
    bson_append_string( &ret, "md5", bson_iterator_string( &it ) );
    bson_destroy( &res );
    if ( contenttype != NULL && *contenttype != '\0' ) {
        bson_append_string( &ret, "contentType", contenttype );
    }
    bson_finish( &ret );
    result = mongo_insert( gfs->client, gfs->files_ns, &ret, NULL );
    bson_destroy( &ret );

    return result;
}
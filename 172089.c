MONGO_EXPORT void gridfs_destroy( gridfs *gfs ) {
    if ( gfs == NULL ) return;
    if ( gfs->dbname ) bson_free( ( char * )gfs->dbname );
    if ( gfs->prefix ) bson_free( ( char * )gfs->prefix );
    if ( gfs->files_ns ) bson_free( ( char * )gfs->files_ns );
    if ( gfs->chunks_ns ) bson_free( ( char * )gfs->chunks_ns );
}
MONGO_EXPORT void gridfile_writer_init( gridfile *gfile, gridfs *gfs,
                                        const char *remote_name, const char *content_type ) {
    gfile->gfs = gfs;

    bson_oid_gen( &( gfile->id ) );
    gfile->chunk_num = 0;
    gfile->length = 0;
    gfile->pending_len = 0;
    gfile->pending_data = NULL;

    gfile->remote_name = ( char * )bson_malloc( strlen( remote_name ) + 1 );
    strcpy( ( char * )gfile->remote_name, remote_name );

    gfile->content_type = ( char * )bson_malloc( strlen( content_type ) + 1 );
    strcpy( ( char * )gfile->content_type, content_type );
}
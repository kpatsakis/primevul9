gridfs_offset gridfile_write_file( gridfile *gfile, FILE *stream ) {
    int i;
    size_t len;
    bson chunk;
    bson_iterator it;
    const char *data;
    const int num = gridfile_get_numchunks( gfile );

    for ( i=0; i<num; i++ ) {
        gridfile_get_chunk( gfile, i, &chunk );
        bson_find( &it, &chunk, "data" );
        len = bson_iterator_bin_len( &it );
        data = bson_iterator_bin_data( &it );
        fwrite( data, sizeof( char ), len, stream );
        bson_destroy( &chunk );
    }

    return gridfile_get_contentlength( gfile );
}
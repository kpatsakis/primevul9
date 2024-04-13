int gridfs_find_query( gridfs *gfs, bson *query,
                       gridfile *gfile ) {

    bson uploadDate;
    bson finalQuery;
    bson out;
    int i;

    bson_init( &uploadDate );
    bson_append_int( &uploadDate, "uploadDate", -1 );
    bson_finish( &uploadDate );

    bson_init( &finalQuery );
    bson_append_bson( &finalQuery, "query", query );
    bson_append_bson( &finalQuery, "orderby", &uploadDate );
    bson_finish( &finalQuery );

    i = ( mongo_find_one( gfs->client, gfs->files_ns,
                          &finalQuery, NULL, &out ) == MONGO_OK );
    bson_destroy( &uploadDate );
    bson_destroy( &finalQuery );
    if ( !i )
        return MONGO_ERROR;
    else {
        gridfile_init( gfs, &out, gfile );
        bson_destroy( &out );
        return MONGO_OK;
    }
}
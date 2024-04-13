int gridfs_find_filename( gridfs *gfs, const char *filename,
                          gridfile *gfile )

{
    bson query;
    int i;

    bson_init( &query );
    bson_append_string( &query, "filename", filename );
    bson_finish( &query );
    i = gridfs_find_query( gfs, &query, gfile );
    bson_destroy( &query );
    return i;
}
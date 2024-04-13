MONGO_EXPORT int gridfile_get_chunksize( gridfile *gfile ) {
    bson_iterator it;

    bson_find( &it, gfile->meta, "chunkSize" );
    return bson_iterator_int( &it );
}
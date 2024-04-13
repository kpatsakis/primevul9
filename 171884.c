MONGO_EXPORT bson_date_t gridfile_get_uploaddate( gridfile *gfile ) {
    bson_iterator it;

    bson_find( &it, gfile->meta, "uploadDate" );
    return bson_iterator_date( &it );
}
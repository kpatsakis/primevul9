MONGO_EXPORT const char *gridfile_get_md5( gridfile *gfile ) {
    bson_iterator it;

    bson_find( &it, gfile->meta, "md5" );
    return bson_iterator_string( &it );
}
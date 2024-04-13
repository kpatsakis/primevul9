MONGO_EXPORT const char *gridfile_get_contenttype( gridfile *gfile ) {
    bson_iterator it;

    if ( bson_find( &it, gfile->meta, "contentType" ) )
        return bson_iterator_string( &it );
    else return NULL;
}
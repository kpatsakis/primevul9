bson_bool_t gridfile_get_boolean( gridfile *gfile, const char *name ) {
    bson_iterator it;

    bson_find( &it, gfile->meta, name );
    return bson_iterator_bool( &it );
}
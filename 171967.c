const char *gridfile_get_field( gridfile *gfile, const char *name ) {
    bson_iterator it;

    bson_find( &it, gfile->meta, name );
    return bson_iterator_value( &it );
}
bson_bool_t gridfile_exists( gridfile *gfile ) {
    return ( bson_bool_t )( gfile != NULL && gfile->meta != NULL );
}
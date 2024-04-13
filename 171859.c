MONGO_EXPORT void gridfile_get_metadata( gridfile *gfile, bson* out ) {
    bson_iterator it;

    if ( bson_find( &it, gfile->meta, "metadata" ) )
        bson_iterator_subobject( &it, out );
    else
        bson_empty( out );
}
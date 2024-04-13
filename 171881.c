MONGO_EXPORT gridfs_offset gridfile_get_contentlength( gridfile *gfile ) {
    bson_iterator it;

    bson_find( &it, gfile->meta, "length" );

    if( bson_iterator_type( &it ) == BSON_INT )
        return ( gridfs_offset )bson_iterator_int( &it );
    else
        return ( gridfs_offset )bson_iterator_long( &it );
}
MONGO_EXPORT int gridfile_get_numchunks( gridfile *gfile ) {
    bson_iterator it;
    gridfs_offset length;
    gridfs_offset chunkSize;
    double numchunks;

    bson_find( &it, gfile->meta, "length" );

    if( bson_iterator_type( &it ) == BSON_INT )
        length = ( gridfs_offset )bson_iterator_int( &it );
    else
        length = ( gridfs_offset )bson_iterator_long( &it );

    bson_find( &it, gfile->meta, "chunkSize" );
    chunkSize = bson_iterator_int( &it );
    numchunks = ( ( double )length/( double )chunkSize );
    return ( numchunks - ( int )numchunks > 0 )
           ? ( int )( numchunks+1 )
           : ( int )( numchunks );
}
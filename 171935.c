static void _bson_init_size( bson *b, int size ) {
    if( size == 0 )
        b->data = NULL;
    else
        b->data = ( char * )bson_malloc( size );
    b->dataSize = size;
    b->cur = b->data + 4;
    _bson_reset( b );
}
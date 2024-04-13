MONGO_EXPORT void bson_iterator_code_scope( const bson_iterator *i, bson *scope ) {
    if ( bson_iterator_type( i ) == BSON_CODEWSCOPE ) {
        int code_len;
        bson_little_endian32( &code_len, bson_iterator_value( i )+4 );
        bson_init_data( scope, ( void * )( bson_iterator_value( i )+8+code_len ) );
        _bson_reset( scope );
        scope->finished = 1;
    }
    else {
        bson_empty( scope );
    }
}
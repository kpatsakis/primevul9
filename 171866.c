MONGO_EXPORT void bson_print_raw( const char *data , int depth ) {
    bson_iterator i;
    const char *key;
    int temp;
    bson_timestamp_t ts;
    char oidhex[25];
    bson scope;
    bson_iterator_from_buffer( &i, data );

    while ( bson_iterator_next( &i ) ) {
        bson_type t = bson_iterator_type( &i );
        if ( t == 0 )
            break;
        key = bson_iterator_key( &i );

        for ( temp=0; temp<=depth; temp++ )
            bson_printf( "\t" );
        bson_printf( "%s : %d \t " , key , t );
        switch ( t ) {
        case BSON_DOUBLE:
            bson_printf( "%f" , bson_iterator_double( &i ) );
            break;
        case BSON_STRING:
            bson_printf( "%s" , bson_iterator_string( &i ) );
            break;
        case BSON_SYMBOL:
            bson_printf( "SYMBOL: %s" , bson_iterator_string( &i ) );
            break;
        case BSON_OID:
            bson_oid_to_string( bson_iterator_oid( &i ), oidhex );
            bson_printf( "%s" , oidhex );
            break;
        case BSON_BOOL:
            bson_printf( "%s" , bson_iterator_bool( &i ) ? "true" : "false" );
            break;
        case BSON_DATE:
            bson_printf( "%ld" , ( long int )bson_iterator_date( &i ) );
            break;
        case BSON_BINDATA:
            bson_printf( "BSON_BINDATA" );
            break;
        case BSON_UNDEFINED:
            bson_printf( "BSON_UNDEFINED" );
            break;
        case BSON_NULL:
            bson_printf( "BSON_NULL" );
            break;
        case BSON_REGEX:
            bson_printf( "BSON_REGEX: %s", bson_iterator_regex( &i ) );
            break;
        case BSON_CODE:
            bson_printf( "BSON_CODE: %s", bson_iterator_code( &i ) );
            break;
        case BSON_CODEWSCOPE:
            bson_printf( "BSON_CODE_W_SCOPE: %s", bson_iterator_code( &i ) );
            /* bson_init( &scope ); */ /* review - stepped on by bson_iterator_code_scope? */
            bson_iterator_code_scope( &i, &scope );
            bson_printf( "\n\t SCOPE: " );
            bson_print( &scope );
            /* bson_destroy( &scope ); */ /* review - causes free error */
            break;
        case BSON_INT:
            bson_printf( "%d" , bson_iterator_int( &i ) );
            break;
        case BSON_LONG:
            bson_printf( "%lld" , ( uint64_t )bson_iterator_long( &i ) );
            break;
        case BSON_TIMESTAMP:
            ts = bson_iterator_timestamp( &i );
            bson_printf( "i: %d, t: %d", ts.i, ts.t );
            break;
        case BSON_OBJECT:
        case BSON_ARRAY:
            bson_printf( "\n" );
            bson_print_raw( bson_iterator_value( &i ) , depth + 1 );
            break;
        default:
            bson_errprintf( "can't print type : %d\n" , t );
        }
        bson_printf( "\n" );
    }
}
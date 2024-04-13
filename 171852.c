void bson_fatal_msg( int ok , const char *msg ) {
    if ( ok )
        return;

    if ( err_handler ) {
        err_handler( msg );
    }
#ifndef R_SAFETY_NET
    bson_errprintf( "error: %s\n" , msg );
    exit( -5 );
#endif
}
void bson_builder_error( bson *b ) {
    if( err_handler )
        err_handler( "BSON error." );
}
static int mongo_cmd_get_error_helper( mongo *conn, const char *db,
                                       bson *realout, const char *cmdtype ) {

    bson out = {NULL,0};
    bson_bool_t haserror = 0;

    /* Reset last error codes. */
    mongo_clear_errors( conn );

    /* If there's an error, store its code and string in the connection object. */
    if( mongo_simple_int_command( conn, db, cmdtype, 1, &out ) == MONGO_OK ) {
        bson_iterator it;
        haserror = ( bson_find( &it, &out, "err" ) != BSON_NULL );
        if( haserror ) mongo_set_last_error( conn, &it, &out );
    }

    if( realout )
        *realout = out; /* transfer of ownership */
    else
        bson_destroy( &out );

    if( haserror )
        return MONGO_ERROR;
    else
        return MONGO_OK;
}
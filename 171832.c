static int mongo_check_last_error( mongo *conn, const char *ns,
                                   mongo_write_concern *write_concern ) {
    int ret = MONGO_OK;
    bson response = {NULL, 0};
    bson fields;
    bson_iterator it;
    int res = 0;
    char *cmd_ns = mongo_ns_to_cmd_db( ns );

    res = mongo_find_one( conn, cmd_ns, write_concern->cmd, bson_empty( &fields ), &response );
    bson_free( cmd_ns );

    if( res != MONGO_OK )
        ret = MONGO_ERROR;
    else {
        if( ( bson_find( &it, &response, "$err" ) == BSON_STRING ) ||
                ( bson_find( &it, &response, "err" ) == BSON_STRING ) ) {

            __mongo_set_error( conn, MONGO_WRITE_ERROR,
                               "See conn->lasterrstr for details.", 0 );
            mongo_set_last_error( conn, &it, &response );
            ret = MONGO_ERROR;
        }
        bson_destroy( &response );
    }
    return ret;
}
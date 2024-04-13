MONGO_EXPORT int mongo_simple_str_command( mongo *conn, const char *db,
        const char *cmdstr, const char *arg, bson *realout ) {

    bson out = {NULL, 0};
    int result;

    bson cmd;
    bson_init( &cmd );
    bson_append_string( &cmd, cmdstr, arg );
    bson_finish( &cmd );

    result = mongo_run_command( conn, db, &cmd, &out );

    bson_destroy( &cmd );

    if ( realout )
        *realout = out;
    else
        bson_destroy( &out );

    return result;
}
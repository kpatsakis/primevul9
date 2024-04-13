MONGO_EXPORT int mongo_simple_int_command( mongo *conn, const char *db,
        const char *cmdstr, int arg, bson *realout ) {

    bson out = {NULL, 0};
    bson cmd;
    int result;

    bson_init( &cmd );
    bson_append_int( &cmd, cmdstr, arg );
    bson_finish( &cmd );

    result = mongo_run_command( conn, db, &cmd, &out );

    bson_destroy( &cmd );

    if ( realout )
        *realout = out;
    else
        bson_destroy( &out );

    return result;
}
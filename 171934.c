MONGO_EXPORT int mongo_create_capped_collection( mongo *conn, const char *db,
        const char *collection, int size, int max, bson *out ) {

    bson b;
    int result;

    bson_init( &b );
    bson_append_string( &b, "create", collection );
    bson_append_bool( &b, "capped", 1 );
    bson_append_int( &b, "size", size );
    if( max > 0 )
        bson_append_int( &b, "max", size );
    bson_finish( &b );

    result = mongo_run_command( conn, db, &b, out );

    bson_destroy( &b );

    return result;
}
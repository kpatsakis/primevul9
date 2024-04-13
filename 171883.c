MONGO_EXPORT int mongo_cmd_add_user( mongo *conn, const char *db, const char *user, const char *pass ) {
    bson user_obj;
    bson pass_obj;
    char hex_digest[33];
    char *ns = bson_malloc( strlen( db ) + strlen( ".system.users" ) + 1 );
    int res;

    strcpy( ns, db );
    strcpy( ns+strlen( db ), ".system.users" );

    mongo_pass_digest( user, pass, hex_digest );

    bson_init( &user_obj );
    bson_append_string( &user_obj, "user", user );
    bson_finish( &user_obj );

    bson_init( &pass_obj );
    bson_append_start_object( &pass_obj, "$set" );
    bson_append_string( &pass_obj, "pwd", hex_digest );
    bson_append_finish_object( &pass_obj );
    bson_finish( &pass_obj );

    res = mongo_update( conn, ns, &user_obj, &pass_obj, MONGO_UPDATE_UPSERT, NULL );

    bson_free( ns );
    bson_destroy( &user_obj );
    bson_destroy( &pass_obj );

    return res;
}
MONGO_EXPORT bson_bool_t mongo_create_simple_index( mongo *conn, const char *ns, const char *field, int options, bson *out ) {
    bson b;
    bson_bool_t success;

    bson_init( &b );
    bson_append_int( &b, field, 1 );
    bson_finish( &b );

    success = mongo_create_index( conn, ns, &b, options, out );
    bson_destroy( &b );
    return success;
}
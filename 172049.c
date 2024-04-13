static void mongo_set_last_error( mongo *conn, bson_iterator *it, bson *obj ) {
    int result_len = bson_iterator_string_len( it );
    const char *result_string = bson_iterator_string( it );
    int len = result_len < MONGO_ERR_LEN ? result_len : MONGO_ERR_LEN;
    memcpy( conn->lasterrstr, result_string, len );

    if( bson_find( it, obj, "code" ) != BSON_NULL )
        conn->lasterrcode = bson_iterator_int( it );
}
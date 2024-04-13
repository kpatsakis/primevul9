static int mongo_read_response( mongo *conn, mongo_reply **reply ) {
    mongo_header head; /* header from network */
    mongo_reply_fields fields; /* header from network */
    mongo_reply *out;  /* native endian */
    unsigned int len;
    int res;

    mongo_env_read_socket( conn, &head, sizeof( head ) );
    mongo_env_read_socket( conn, &fields, sizeof( fields ) );

    bson_little_endian32( &len, &head.len );

    if ( len < sizeof( head )+sizeof( fields ) || len > 64*1024*1024 )
        return MONGO_READ_SIZE_ERROR;  /* most likely corruption */

    out = ( mongo_reply * )bson_malloc( len );

    out->head.len = len;
    bson_little_endian32( &out->head.id, &head.id );
    bson_little_endian32( &out->head.responseTo, &head.responseTo );
    bson_little_endian32( &out->head.op, &head.op );

    bson_little_endian32( &out->fields.flag, &fields.flag );
    bson_little_endian64( &out->fields.cursorID, &fields.cursorID );
    bson_little_endian32( &out->fields.start, &fields.start );
    bson_little_endian32( &out->fields.num, &fields.num );

    res = mongo_env_read_socket( conn, &out->objs, len-sizeof( head )-sizeof( fields ) );
    if( res != MONGO_OK ) {
        bson_free( out );
        return res;
    }

    *reply = out;

    return MONGO_OK;
}
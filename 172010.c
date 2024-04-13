MONGO_EXPORT int bson_copy( bson *out, const bson *in ) {
    if ( !out || !in ) return BSON_ERROR;
    if ( !in->finished ) return BSON_ERROR;
    bson_init_size( out, bson_size( in ) );
    memcpy( out->data, in->data, bson_size( in ) );
    out->finished = 1;

    return BSON_OK;
}
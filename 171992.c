MONGO_EXPORT time_t bson_oid_generated_time( bson_oid_t *oid ) {
    time_t out;
    bson_big_endian32( &out, &oid->ints[0] );

    return out;
}
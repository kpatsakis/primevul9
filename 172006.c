static char *mongo_data_append64( char *start , const void *data ) {
    bson_little_endian64( start , data );
    return start + 8;
}
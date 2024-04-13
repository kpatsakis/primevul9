static char *mongo_data_append( char *start , const void *data , size_t len ) {
    memcpy( start , data , len );
    return start + len;
}
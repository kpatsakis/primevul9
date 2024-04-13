MONGO_EXPORT bson *bson_empty( bson *obj ) {
    static char *data = "\005\0\0\0\0";
    bson_init_data( obj, data );
    obj->finished = 1;
    obj->err = 0;
    obj->errstr = NULL;
    obj->stackPos = 0;
    return obj;
}
MONGO_EXPORT int mongo_replset_connect( mongo *conn ) {
    int ret;
    bson_errprintf("WARNING: mongo_replset_connect() is deprecated, please use mongo_replica_set_client()\n");
    ret = mongo_replica_set_client( conn );
    mongo_set_write_concern( conn, 0 );
    return ret;
}
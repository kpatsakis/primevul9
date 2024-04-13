MONGO_EXPORT void mongo_replset_init( mongo *conn, const char *name ) {
    bson_errprintf("WARNING: mongo_replset_init() is deprecated, please use mongo_replica_set_init()\n");
    mongo_replica_set_init( conn, name );
}
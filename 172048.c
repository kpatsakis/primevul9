MONGO_EXPORT void mongo_replset_add_seed( mongo *conn, const char *host, int port ) {
    bson_errprintf("WARNING: mongo_replset_add_seed() is deprecated, please use mongo_replica_set_add_seed()\n");
    mongo_replica_set_add_node( &conn->replica_set->seeds, host, port );
}
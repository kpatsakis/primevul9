MONGO_EXPORT void mongo_replica_set_add_seed( mongo *conn, const char *host, int port ) {
    mongo_replica_set_add_node( &conn->replica_set->seeds, host, port );
}
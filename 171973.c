MONGO_EXPORT void mongo_replica_set_init( mongo *conn, const char *name ) {
    mongo_init( conn );

    conn->replica_set = bson_malloc( sizeof( mongo_replica_set ) );
    conn->replica_set->primary_connected = 0;
    conn->replica_set->seeds = NULL;
    conn->replica_set->hosts = NULL;
    conn->replica_set->name = ( char * )bson_malloc( strlen( name ) + 1 );
    memcpy( conn->replica_set->name, name, strlen( name ) + 1  );

    conn->primary = bson_malloc( sizeof( mongo_host_port ) );
    conn->primary->host[0] = '\0';
    conn->primary->next = NULL;
}
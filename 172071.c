MONGO_EXPORT int mongo_reconnect( mongo *conn ) {
    int res;
    mongo_disconnect( conn );

    if( conn->replica_set ) {
        conn->replica_set->primary_connected = 0;
        mongo_replica_set_free_list( &conn->replica_set->hosts );
        conn->replica_set->hosts = NULL;
        res = mongo_replica_set_client( conn );
        return res;
    }
    else
        return mongo_env_socket_connect( conn, conn->primary->host, conn->primary->port );
}
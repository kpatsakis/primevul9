MONGO_EXPORT void mongo_disconnect( mongo *conn ) {
    if( ! conn->connected )
        return;

    if( conn->replica_set ) {
        conn->replica_set->primary_connected = 0;
        mongo_replica_set_free_list( &conn->replica_set->hosts );
        conn->replica_set->hosts = NULL;
    }

    mongo_env_close_socket( conn->sock );

    conn->sock = 0;
    conn->connected = 0;
}
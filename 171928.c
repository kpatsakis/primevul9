MONGO_EXPORT void mongo_destroy( mongo *conn ) {
    mongo_disconnect( conn );

    if( conn->replica_set ) {
        mongo_replica_set_free_list( &conn->replica_set->seeds );
        mongo_replica_set_free_list( &conn->replica_set->hosts );
        bson_free( conn->replica_set->name );
        bson_free( conn->replica_set );
        conn->replica_set = NULL;
    }

    bson_free( conn->primary );

    mongo_clear_errors( conn );
}
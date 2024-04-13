static void mongo_replica_set_free_list( mongo_host_port **list ) {
    mongo_host_port *node = *list;
    mongo_host_port *prev;

    while( node != NULL ) {
        prev = node;
        node = node->next;
        bson_free( prev );
    }

    *list = NULL;
}
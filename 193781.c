crm_client_cleanup(void)
{
    if (client_connections == NULL) {
        int active = g_hash_table_size(client_connections);

        if (active) {
            crm_err("Exiting with %d active connections", active);
        }
        g_hash_table_destroy(client_connections);
    }
}
crm_client_init(void)
{
    if (client_connections == NULL) {
        crm_trace("Creating client hash table");
        client_connections = g_hash_table_new(g_direct_hash, g_direct_equal);
    }
}
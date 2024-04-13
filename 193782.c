crm_client_get(qb_ipcs_connection_t * c)
{
    if (client_connections) {
        return g_hash_table_lookup(client_connections, c);
    }

    crm_trace("No client found for %p", c);
    return NULL;
}
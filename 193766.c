crm_client_get_by_id(const char *id)
{
    gpointer key;
    crm_client_t *client;
    GHashTableIter iter;

    if (client_connections && id) {
        g_hash_table_iter_init(&iter, client_connections);
        while (g_hash_table_iter_next(&iter, &key, (gpointer *) & client)) {
            if (strcmp(client->id, id) == 0) {
                return client;
            }
        }
    }

    crm_trace("No client found with id=%s", id);
    return NULL;
}
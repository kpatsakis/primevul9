crm_client_destroy(crm_client_t * c)
{
    if (c == NULL) {
        return;
    }

    if (client_connections) {
        if (c->ipcs) {
            crm_trace("Destroying %p/%p (%d remaining)",
                      c, c->ipcs, crm_hash_table_size(client_connections) - 1);
            g_hash_table_remove(client_connections, c->ipcs);

        } else {
            crm_trace("Destroying remote connection %p (%d remaining)",
                      c, crm_hash_table_size(client_connections) - 1);
            g_hash_table_remove(client_connections, c->id);
        }
    }

    if (c->event_timer) {
        g_source_remove(c->event_timer);
    }

    crm_info("Destroying %d events", g_list_length(c->event_queue));
    while (c->event_queue) {
        struct iovec *event = c->event_queue->data;

        c->event_queue = g_list_remove(c->event_queue, event);
        free(event[0].iov_base);
        free(event[1].iov_base);
        free(event);
    }

    free(c->id);
    free(c->name);
    free(c->user);
    if (c->remote) {
        if (c->remote->auth_timeout) {
            g_source_remove(c->remote->auth_timeout);
        }
        free(c->remote->buffer);
        free(c->remote);
    }
    free(c);
}
crm_ipcs_flush_events_cb(gpointer data)
{
    crm_client_t *c = data;

    c->event_timer = 0;
    crm_ipcs_flush_events(c);
    return FALSE;
}
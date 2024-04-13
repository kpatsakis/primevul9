crm_ipc_connect(crm_ipc_t * client)
{
    client->need_reply = FALSE;
    client->ipc = qb_ipcc_connect(client->name, client->buf_size);

    if (client->ipc == NULL) {
        crm_perror(LOG_INFO, "Could not establish %s connection", client->name);
        return FALSE;
    }

    client->pfd.fd = crm_ipc_get_fd(client);
    if (client->pfd.fd < 0) {
        crm_perror(LOG_INFO, "Could not obtain file descriptor for %s connection", client->name);
        return FALSE;
    }

    qb_ipcc_context_set(client->ipc, client);

    return TRUE;
}
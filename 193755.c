crm_ipc_ready(crm_ipc_t * client)
{
    CRM_ASSERT(client != NULL);

    if (crm_ipc_connected(client) == FALSE) {
        return -ENOTCONN;
    }

    client->pfd.revents = 0;
    return poll(&(client->pfd), 1, 0);
}
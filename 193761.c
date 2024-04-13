crm_ipc_close(crm_ipc_t * client)
{
    if (client) {
        crm_trace("Disconnecting %s IPC connection %p (%p.%p)", client->name, client, client->ipc);

        if (client->ipc) {
            qb_ipcc_connection_t *ipc = client->ipc;

            client->ipc = NULL;
            qb_ipcc_disconnect(ipc);
        }
    }
}
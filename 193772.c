crm_ipc_connected(crm_ipc_t * client)
{
    bool rc = FALSE;

    if (client == NULL) {
        crm_trace("No client");
        return FALSE;

    } else if (client->ipc == NULL) {
        crm_trace("No connection");
        return FALSE;

    } else if (client->pfd.fd < 0) {
        crm_trace("Bad descriptor");
        return FALSE;
    }

    rc = qb_ipcc_is_connected(client->ipc);
    if (rc == FALSE) {
        client->pfd.fd = -EINVAL;
    }
    return rc;
}
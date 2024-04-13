crm_ipc_get_fd(crm_ipc_t * client)
{
    int fd = 0;

    CRM_ASSERT(client != NULL);
    if (client->ipc && qb_ipcc_fd_get(client->ipc, &fd) == 0) {
        return fd;
    }

    crm_perror(LOG_ERR, "Could not obtain file IPC descriptor for %s", client->name);
    return -EINVAL;
}
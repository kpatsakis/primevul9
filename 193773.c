crm_ipcs_send(crm_client_t * c, uint32_t request, xmlNode * message,
              enum crm_ipc_server_flags flags)
{
    struct iovec *iov = NULL;
    ssize_t rc = crm_ipc_prepare(request, message, &iov);

    if (rc > 0) {
        rc = crm_ipcs_sendv(c, iov, flags | crm_ipc_server_free);

    } else {
        crm_notice("Message to %p[%d] failed: %s (%d)", c->ipcs, c->pid, pcmk_strerror(rc), rc);
    }

    return rc;
}
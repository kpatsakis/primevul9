internal_ipc_send_recv(crm_ipc_t * client, const void *iov)
{
    int rc = 0;

    do {
        rc = qb_ipcc_sendv_recv(client->ipc, iov, 2, client->buffer, client->buf_size, -1);
    } while (rc == -EAGAIN && crm_ipc_connected(client));

    return rc;
}
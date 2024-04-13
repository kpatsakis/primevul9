internal_ipc_send_request(crm_ipc_t * client, const void *iov, int ms_timeout)
{
    int rc = 0;
    time_t timeout = time(NULL) + 1 + (ms_timeout / 1000);

    do {
        rc = qb_ipcc_sendv(client->ipc, iov, 2);
    } while (rc == -EAGAIN && time(NULL) < timeout && crm_ipc_connected(client));

    return rc;
}
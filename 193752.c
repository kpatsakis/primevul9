crm_ipcs_sendv(crm_client_t * c, struct iovec * iov, enum crm_ipc_server_flags flags)
{
    ssize_t rc;
    static uint32_t id = 1;
    struct crm_ipc_response_header *header = iov[0].iov_base;

    if (flags & crm_ipc_server_event) {
        header->qb.id = id++;   /* We don't really use it, but doesn't hurt to set one */

        if (flags & crm_ipc_server_free) {
            crm_trace("Sending the original to %p[%d]", c->ipcs, c->pid);
            c->event_queue = g_list_append(c->event_queue, iov);

        } else {
            struct iovec *iov_copy = calloc(2, sizeof(struct iovec));

            crm_trace("Sending a copy to %p[%d]", c->ipcs, c->pid);
            iov_copy[0].iov_len = iov[0].iov_len;
            iov_copy[0].iov_base = malloc(iov[0].iov_len);
            memcpy(iov_copy[0].iov_base, iov[0].iov_base, iov[0].iov_len);

            iov_copy[1].iov_len = iov[1].iov_len;
            iov_copy[1].iov_base = malloc(iov[1].iov_len);
            memcpy(iov_copy[1].iov_base, iov[1].iov_base, iov[1].iov_len);

            c->event_queue = g_list_append(c->event_queue, iov_copy);
        }

    } else {
        CRM_LOG_ASSERT(header->qb.id != 0);     /* Replying to a specific request */

        rc = qb_ipcs_response_sendv(c->ipcs, iov, 2);
        if (rc < header->qb.size) {
            crm_notice("Response %d to %p[%d] (%d bytes) failed: %s (%d)",
                       header->qb.id, c->ipcs, c->pid, header->qb.size, pcmk_strerror(rc), rc);

        } else {
            crm_trace("Response %d sent, %d bytes to %p[%d]", header->qb.id, rc, c->ipcs, c->pid);
        }

        if (flags & crm_ipc_server_free) {
            free(iov[0].iov_base);
            free(iov[1].iov_base);
            free(iov);
        }
    }

    if (flags & crm_ipc_server_event) {
        rc = crm_ipcs_flush_events(c);
    } else {
        crm_ipcs_flush_events(c);
    }

    if (rc == -EPIPE || rc == -ENOTCONN) {
        crm_trace("Client %p disconnected", c->ipcs);
    }

    return rc;
}
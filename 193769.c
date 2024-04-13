crm_ipc_send(crm_ipc_t * client, xmlNode * message, enum crm_ipc_flags flags, int32_t ms_timeout,
             xmlNode ** reply)
{
    long rc = 0;
    struct iovec *iov;
    static uint32_t id = 0;
    struct crm_ipc_response_header *header;

    crm_ipc_init();

    if (client == NULL) {
        crm_notice("Invalid connection");
        return -ENOTCONN;

    } else if (crm_ipc_connected(client) == FALSE) {
        /* Don't even bother */
        crm_notice("Connection to %s closed", client->name);
        return -ENOTCONN;
    }

    if (client->need_reply) {
        crm_trace("Trying again to obtain pending reply from %s", client->name);
        rc = qb_ipcc_recv(client->ipc, client->buffer, client->buf_size, 300);
        if (rc < 0) {
            crm_warn("Sending to %s (%p) is disabled until pending reply is recieved", client->name,
                     client->ipc);
            return -EALREADY;

        } else {
            crm_notice("Lost reply from %s (%p) finally arrived, sending re-enabled", client->name,
                       client->ipc);
            client->need_reply = FALSE;
        }
    }

    rc = crm_ipc_prepare(++id, message, &iov);
    if(rc < 0) {
        return rc;
    }

    header = iov[0].iov_base;
    header->flags |= flags;

    if (ms_timeout == 0) {
        ms_timeout = 5000;
    }

    crm_trace("Sending from client: %s request id: %d bytes: %u timeout:%d msg...",
              client->name, header->qb.id, header->qb.size, ms_timeout);

    if (ms_timeout > 0) {

        rc = internal_ipc_send_request(client, iov, ms_timeout);

        if (rc <= 0) {
            crm_trace("Failed to send from client %s request %d with %u bytes...",
                      client->name, header->qb.id, header->qb.size);
            goto send_cleanup;

        } else if (is_not_set(flags, crm_ipc_client_response)) {
            crm_trace("Message sent, not waiting for reply to %d from %s to %u bytes...",
                      header->qb.id, client->name, header->qb.size);

            goto send_cleanup;
        }

        rc = internal_ipc_get_reply(client, header->qb.id, ms_timeout);
        if (rc < 0) {
            /* No reply, for now, disable sending
             *
             * The alternative is to close the connection since we don't know
             * how to detect and discard out-of-sequence replies
             *
             * TODO - implement the above
             */
            client->need_reply = TRUE;
        }

    } else {
        rc = internal_ipc_send_recv(client, iov);
    }

    if (rc > 0) {
        struct crm_ipc_response_header *hdr = (struct crm_ipc_response_header *)client->buffer;

        crm_trace("Recieved response %d, size=%d, rc=%ld, text: %.200s", hdr->qb.id, hdr->qb.size,
                  rc, crm_ipc_buffer(client));

        if (reply) {
            *reply = string2xml(crm_ipc_buffer(client));
        }

    } else {
        crm_trace("Response not recieved: rc=%ld, errno=%d", rc, errno);
    }

  send_cleanup:
    if (crm_ipc_connected(client) == FALSE) {
        crm_notice("Connection to %s closed: %s (%ld)", client->name, pcmk_strerror(rc), rc);

    } else if (rc == -ETIMEDOUT) {
        crm_warn("Request %d to %s (%p) failed: %s (%ld) after %dms",
                 header->qb.id, client->name, client->ipc, pcmk_strerror(rc), rc, ms_timeout);
        crm_write_blackbox(0, NULL);

    } else if (rc <= 0) {
        crm_warn("Request %d to %s (%p) failed: %s (%ld)",
                 header->qb.id, client->name, client->ipc, pcmk_strerror(rc), rc);
    }

    free(header);
    free(iov[1].iov_base);
    free(iov);
    return rc;
}
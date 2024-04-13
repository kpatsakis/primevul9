internal_ipc_get_reply(crm_ipc_t * client, int request_id, int ms_timeout)
{
    time_t timeout = time(NULL) + 1 + (ms_timeout / 1000);
    int rc = 0;

    crm_ipc_init();

    /* get the reply */
    crm_trace("client %s waiting on reply to msg id %d", client->name, request_id);
    do {

        rc = qb_ipcc_recv(client->ipc, client->buffer, client->buf_size, 1000);
        if (rc > 0) {
            struct crm_ipc_response_header *hdr = NULL;

            int rc = crm_ipc_decompress(client);

            if (rc != pcmk_ok) {
                return rc;
            }

            hdr = (struct crm_ipc_response_header *)client->buffer;
            if (hdr->qb.id == request_id) {
                /* Got it */
                break;
            } else if (hdr->qb.id < request_id) {
                xmlNode *bad = string2xml(crm_ipc_buffer(client));

                crm_err("Discarding old reply %d (need %d)", hdr->qb.id, request_id);
                crm_log_xml_notice(bad, "OldIpcReply");

            } else {
                xmlNode *bad = string2xml(crm_ipc_buffer(client));

                crm_err("Discarding newer reply %d (need %d)", hdr->qb.id, request_id);
                crm_log_xml_notice(bad, "ImpossibleReply");
                CRM_ASSERT(hdr->qb.id <= request_id);
            }
        } else if (crm_ipc_connected(client) == FALSE) {
            crm_err("Server disconnected client %s while waiting for msg id %d", client->name,
                    request_id);
            break;
        }

    } while (time(NULL) < timeout);

    return rc;
}
crm_ipcs_flush_events(crm_client_t * c)
{
    int sent = 0;
    ssize_t rc = 0;
    int queue_len = 0;

    if (c == NULL) {
        return pcmk_ok;

    } else if (c->event_timer) {
        /* There is already a timer, wait until it goes off */
        crm_trace("Timer active for %p - %d", c->ipcs, c->event_timer);
        return pcmk_ok;
    }

    queue_len = g_list_length(c->event_queue);
    while (c->event_queue && sent < 100) {
        struct crm_ipc_response_header *header = NULL;
        struct iovec *event = c->event_queue->data;

        rc = qb_ipcs_event_sendv(c->ipcs, event, 2);
        if (rc < 0) {
            break;
        }

        sent++;
        header = event[0].iov_base;
        if (header->flags & crm_ipc_compressed) {
            crm_trace("Event %d to %p[%d] (%d compressed bytes) sent",
                      header->qb.id, c->ipcs, c->pid, rc);
        } else {
            crm_trace("Event %d to %p[%d] (%d bytes) sent: %.120s",
                      header->qb.id, c->ipcs, c->pid, rc, event[1].iov_base);
        }

        c->event_queue = g_list_remove(c->event_queue, event);
        free(event[0].iov_base);
        free(event[1].iov_base);
        free(event);
    }

    queue_len -= sent;
    if (sent > 0 || c->event_queue) {
        crm_trace("Sent %d events (%d remaining) for %p[%d]: %s",
                  sent, queue_len, c->ipcs, c->pid, pcmk_strerror(rc < 0 ? rc : 0));
    }

    if (c->event_queue) {
        if (queue_len % 100 == 0 && queue_len > 99) {
            crm_warn("Event queue for %p[%d] has grown to %d", c->ipcs, c->pid, queue_len);

        } else if (queue_len > 500) {
            crm_err("Evicting slow client %p[%d]: event queue reached %d entries",
                    c->ipcs, c->pid, queue_len);
            qb_ipcs_disconnect(c->ipcs);
            return rc;
        }

        c->event_timer = g_timeout_add(1000 + 100 * queue_len, crm_ipcs_flush_events_cb, c);
    }

    return rc;
}
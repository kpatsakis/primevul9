static int dns_stream_update_io(DnsStream *s) {
        int f = 0;

        assert(s);

        if (s->write_packet && s->n_written < sizeof(s->write_size) + s->write_packet->size)
                f |= EPOLLOUT;
        else if (!ordered_set_isempty(s->write_queue)) {
                dns_packet_unref(s->write_packet);
                s->write_packet = ordered_set_steal_first(s->write_queue);
                s->write_size = htobe16(s->write_packet->size);
                s->n_written = 0;
                f |= EPOLLOUT;
        }
        if (!s->read_packet || s->n_read < sizeof(s->read_size) + s->read_packet->size)
                f |= EPOLLIN;

#if ENABLE_DNS_OVER_TLS
        /* For handshake and clean closing purposes, TLS can override requested events */
        if (s->dnstls_events)
                f = s->dnstls_events;
#endif

        return sd_event_source_set_io_events(s->io_event_source, f);
}
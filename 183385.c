int dns_stream_new(
                Manager *m,
                DnsStream **ret,
                DnsProtocol protocol,
                int fd,
                const union sockaddr_union *tfo_address) {

        _cleanup_(dns_stream_unrefp) DnsStream *s = NULL;
        int r;

        assert(m);
        assert(ret);
        assert(fd >= 0);

        if (m->n_dns_streams > DNS_STREAMS_MAX)
                return -EBUSY;

        s = new(DnsStream, 1);
        if (!s)
                return -ENOMEM;

        *s = (DnsStream) {
                .n_ref = 1,
                .fd = -1,
                .protocol = protocol,
        };

        r = ordered_set_ensure_allocated(&s->write_queue, &dns_packet_hash_ops);
        if (r < 0)
                return r;

        r = sd_event_add_io(m->event, &s->io_event_source, fd, EPOLLIN, on_stream_io, s);
        if (r < 0)
                return r;

        (void) sd_event_source_set_description(s->io_event_source, "dns-stream-io");

        r = sd_event_add_time(
                        m->event,
                        &s->timeout_event_source,
                        clock_boottime_or_monotonic(),
                        now(clock_boottime_or_monotonic()) + DNS_STREAM_TIMEOUT_USEC, 0,
                        on_stream_timeout, s);
        if (r < 0)
                return r;

        (void) sd_event_source_set_description(s->timeout_event_source, "dns-stream-timeout");

        LIST_PREPEND(streams, m->dns_streams, s);
        m->n_dns_streams++;
        s->manager = m;

        s->fd = fd;

        if (tfo_address) {
                s->tfo_address = *tfo_address;
                s->tfo_salen = tfo_address->sa.sa_family == AF_INET6 ? sizeof(tfo_address->in6) : sizeof(tfo_address->in);
        }

        *ret = TAKE_PTR(s);

        return 0;
}
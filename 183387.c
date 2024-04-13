static void dns_stream_stop(DnsStream *s) {
        assert(s);

        s->io_event_source = sd_event_source_unref(s->io_event_source);
        s->timeout_event_source = sd_event_source_unref(s->timeout_event_source);
        s->fd = safe_close(s->fd);
}
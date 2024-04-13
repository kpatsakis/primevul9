static DnsStream *dns_stream_free(DnsStream *s) {
        DnsPacket *p;
        Iterator i;

        assert(s);

        dns_stream_stop(s);

        if (s->server && s->server->stream == s)
                s->server->stream = NULL;

        if (s->manager) {
                LIST_REMOVE(streams, s->manager->dns_streams, s);
                s->manager->n_dns_streams--;
        }

#if ENABLE_DNS_OVER_TLS
        if (s->encrypted)
                dnstls_stream_free(s);
#endif

        ORDERED_SET_FOREACH(p, s->write_queue, i)
                dns_packet_unref(ordered_set_remove(s->write_queue, p));

        dns_packet_unref(s->write_packet);
        dns_packet_unref(s->read_packet);
        dns_server_unref(s->server);

        ordered_set_free(s->write_queue);

        return mfree(s);
}
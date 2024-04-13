DnsPacket *dns_stream_take_read_packet(DnsStream *s) {
        assert(s);

        if (!s->read_packet)
                return NULL;

        if (s->n_read < sizeof(s->read_size))
                return NULL;

        if (s->n_read < sizeof(s->read_size) + be16toh(s->read_size))
                return NULL;

        s->n_read = 0;
        return TAKE_PTR(s->read_packet);
}
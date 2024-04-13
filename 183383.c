static ssize_t dns_stream_read(DnsStream *s, void *buf, size_t count) {
        ssize_t ss;

#if ENABLE_DNS_OVER_TLS
        if (s->encrypted)
                ss = dnstls_stream_read(s, buf, count);
        else
#endif
        {
                ss = read(s->fd, buf, count);
                if (ss < 0)
                        return -errno;
        }

        return ss;
}
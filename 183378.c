ssize_t dns_stream_writev(DnsStream *s, const struct iovec *iov, size_t iovcnt, int flags) {
        ssize_t m;

        assert(s);
        assert(iov);

#if ENABLE_DNS_OVER_TLS
        if (s->encrypted && !(flags & DNS_STREAM_WRITE_TLS_DATA)) {
                ssize_t ss;
                size_t i;

                m = 0;
                for (i = 0; i < iovcnt; i++) {
                        ss = dnstls_stream_write(s, iov[i].iov_base, iov[i].iov_len);
                        if (ss < 0)
                                return ss;

                        m += ss;
                        if (ss != (ssize_t) iov[i].iov_len)
                                continue;
                }
        } else
#endif
        if (s->tfo_salen > 0) {
                struct msghdr hdr = {
                        .msg_iov = (struct iovec*) iov,
                        .msg_iovlen = iovcnt,
                        .msg_name = &s->tfo_address.sa,
                        .msg_namelen = s->tfo_salen
                };

                m = sendmsg(s->fd, &hdr, MSG_FASTOPEN);
                if (m < 0) {
                        if (errno == EOPNOTSUPP) {
                                s->tfo_salen = 0;
                                if (connect(s->fd, &s->tfo_address.sa, s->tfo_salen) < 0)
                                        return -errno;

                                return -EAGAIN;
                        }
                        if (errno == EINPROGRESS)
                                return -EAGAIN;

                        return -errno;
                } else
                        s->tfo_salen = 0; /* connection is made */
        } else {
                m = writev(s->fd, iov, iovcnt);
                if (m < 0)
                        return -errno;
        }

        return m;
}
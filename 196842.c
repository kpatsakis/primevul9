static int add_iov(conn *c, const void *buf, int len) {
    struct msghdr *m;
    int leftover;

    assert(c != NULL);

    if (IS_UDP(c->transport)) {
        do {
            m = &c->msglist[c->msgused - 1];

            /*
             * Limit UDP packets to UDP_MAX_PAYLOAD_SIZE bytes.
             */

            /* We may need to start a new msghdr if this one is full. */
            if (m->msg_iovlen == IOV_MAX ||
                (c->msgbytes >= UDP_MAX_PAYLOAD_SIZE)) {
                add_msghdr(c);
                m = &c->msglist[c->msgused - 1];
            }

            if (ensure_iov_space(c) != 0)
                return -1;

            /* If the fragment is too big to fit in the datagram, split it up */
            if (len + c->msgbytes > UDP_MAX_PAYLOAD_SIZE) {
                leftover = len + c->msgbytes - UDP_MAX_PAYLOAD_SIZE;
                len -= leftover;
            } else {
                leftover = 0;
            }

            m = &c->msglist[c->msgused - 1];
            m->msg_iov[m->msg_iovlen].iov_base = (void *)buf;
            m->msg_iov[m->msg_iovlen].iov_len = len;

            c->msgbytes += len;
            c->iovused++;
            m->msg_iovlen++;

            buf = ((char *)buf) + len;
            len = leftover;
        } while (leftover > 0);
    } else {
        /* Optimized path for TCP connections */
        m = &c->msglist[c->msgused - 1];
        if (m->msg_iovlen == IOV_MAX) {
            add_msghdr(c);
            m = &c->msglist[c->msgused - 1];
        }

        if (ensure_iov_space(c) != 0)
            return -1;

        m->msg_iov[m->msg_iovlen].iov_base = (void *)buf;
        m->msg_iov[m->msg_iovlen].iov_len = len;
        c->msgbytes += len;
        c->iovused++;
        m->msg_iovlen++;
    }

    return 0;
}
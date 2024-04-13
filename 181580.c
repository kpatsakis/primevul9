void bootp_input(struct mbuf *m)
{
    struct bootp_t *bp = mtod_check(m, sizeof(struct bootp_t));

    if (bp && bp->bp_op == BOOTP_REQUEST) {
        bootp_reply(m->slirp, bp, m_end(m));
    }
}
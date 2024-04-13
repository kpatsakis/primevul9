int udp6_output(struct socket *so, struct mbuf *m, struct sockaddr_in6 *saddr,
                struct sockaddr_in6 *daddr)
{
    Slirp *slirp = m->slirp;
    M_DUP_DEBUG(slirp, m, 0, sizeof(struct ip6) + sizeof(struct udphdr));

    struct ip6 *ip;
    struct udphdr *uh;

    DEBUG_CALL("udp6_output");
    DEBUG_ARG("so = %p", so);
    DEBUG_ARG("m = %p", m);

    /* adjust for header */
    m->m_data -= sizeof(struct udphdr);
    m->m_len += sizeof(struct udphdr);
    uh = mtod(m, struct udphdr *);
    m->m_data -= sizeof(struct ip6);
    m->m_len += sizeof(struct ip6);
    ip = mtod(m, struct ip6 *);

    /* Build IP header */
    ip->ip_pl = htons(m->m_len - sizeof(struct ip6));
    ip->ip_nh = IPPROTO_UDP;
    ip->ip_src = saddr->sin6_addr;
    ip->ip_dst = daddr->sin6_addr;

    /* Build UDP header */
    uh->uh_sport = saddr->sin6_port;
    uh->uh_dport = daddr->sin6_port;
    uh->uh_ulen = ip->ip_pl;
    uh->uh_sum = 0;
    uh->uh_sum = ip6_cksum(m);
    if (uh->uh_sum == 0) {
        uh->uh_sum = 0xffff;
    }

    return ip6_output(so, m, 0);
}
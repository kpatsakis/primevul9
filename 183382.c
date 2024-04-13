static int dns_stream_identify(DnsStream *s) {
        union {
                struct cmsghdr header; /* For alignment */
                uint8_t buffer[CMSG_SPACE(MAXSIZE(struct in_pktinfo, struct in6_pktinfo))
                               + EXTRA_CMSG_SPACE /* kernel appears to require extra space */];
        } control;
        struct msghdr mh = {};
        struct cmsghdr *cmsg;
        socklen_t sl;
        int r;

        assert(s);

        if (s->identified)
                return 0;

        /* Query the local side */
        s->local_salen = sizeof(s->local);
        r = getsockname(s->fd, &s->local.sa, &s->local_salen);
        if (r < 0)
                return -errno;
        if (s->local.sa.sa_family == AF_INET6 && s->ifindex <= 0)
                s->ifindex = s->local.in6.sin6_scope_id;

        /* Query the remote side */
        s->peer_salen = sizeof(s->peer);
        r = getpeername(s->fd, &s->peer.sa, &s->peer_salen);
        if (r < 0)
                return -errno;
        if (s->peer.sa.sa_family == AF_INET6 && s->ifindex <= 0)
                s->ifindex = s->peer.in6.sin6_scope_id;

        /* Check consistency */
        assert(s->peer.sa.sa_family == s->local.sa.sa_family);
        assert(IN_SET(s->peer.sa.sa_family, AF_INET, AF_INET6));

        /* Query connection meta information */
        sl = sizeof(control);
        if (s->peer.sa.sa_family == AF_INET) {
                r = getsockopt(s->fd, IPPROTO_IP, IP_PKTOPTIONS, &control, &sl);
                if (r < 0)
                        return -errno;
        } else if (s->peer.sa.sa_family == AF_INET6) {

                r = getsockopt(s->fd, IPPROTO_IPV6, IPV6_2292PKTOPTIONS, &control, &sl);
                if (r < 0)
                        return -errno;
        } else
                return -EAFNOSUPPORT;

        mh.msg_control = &control;
        mh.msg_controllen = sl;

        CMSG_FOREACH(cmsg, &mh) {

                if (cmsg->cmsg_level == IPPROTO_IPV6) {
                        assert(s->peer.sa.sa_family == AF_INET6);

                        switch (cmsg->cmsg_type) {

                        case IPV6_PKTINFO: {
                                struct in6_pktinfo *i = (struct in6_pktinfo*) CMSG_DATA(cmsg);

                                if (s->ifindex <= 0)
                                        s->ifindex = i->ipi6_ifindex;
                                break;
                        }

                        case IPV6_HOPLIMIT:
                                s->ttl = *(int *) CMSG_DATA(cmsg);
                                break;
                        }

                } else if (cmsg->cmsg_level == IPPROTO_IP) {
                        assert(s->peer.sa.sa_family == AF_INET);

                        switch (cmsg->cmsg_type) {

                        case IP_PKTINFO: {
                                struct in_pktinfo *i = (struct in_pktinfo*) CMSG_DATA(cmsg);

                                if (s->ifindex <= 0)
                                        s->ifindex = i->ipi_ifindex;
                                break;
                        }

                        case IP_TTL:
                                s->ttl = *(int *) CMSG_DATA(cmsg);
                                break;
                        }
                }
        }

        /* The Linux kernel sets the interface index to the loopback
         * device if the connection came from the local host since it
         * avoids the routing table in such a case. Let's unset the
         * interface index in such a case. */
        if (s->ifindex == LOOPBACK_IFINDEX)
                s->ifindex = 0;

        /* If we don't know the interface index still, we look for the
         * first local interface with a matching address. Yuck! */
        if (s->ifindex <= 0)
                s->ifindex = manager_find_ifindex(s->manager, s->local.sa.sa_family, s->local.sa.sa_family == AF_INET ? (union in_addr_union*) &s->local.in.sin_addr : (union in_addr_union*)  &s->local.in6.sin6_addr);

        if (s->protocol == DNS_PROTOCOL_LLMNR && s->ifindex > 0) {
                uint32_t ifindex = htobe32(s->ifindex);

                /* Make sure all packets for this connection are sent on the same interface */
                if (s->local.sa.sa_family == AF_INET) {
                        r = setsockopt(s->fd, IPPROTO_IP, IP_UNICAST_IF, &ifindex, sizeof(ifindex));
                        if (r < 0)
                                log_debug_errno(errno, "Failed to invoke IP_UNICAST_IF: %m");
                } else if (s->local.sa.sa_family == AF_INET6) {
                        r = setsockopt(s->fd, IPPROTO_IPV6, IPV6_UNICAST_IF, &ifindex, sizeof(ifindex));
                        if (r < 0)
                                log_debug_errno(errno, "Failed to invoke IPV6_UNICAST_IF: %m");
                }
        }

        s->identified = true;

        return 0;
}
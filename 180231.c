static int network_bind_socket(int fd, const struct addrinfo *ai,
                               const int interface_idx) {
#if KERNEL_SOLARIS
  char loop = 0;
#else
  int loop = 0;
#endif
  int yes = 1;

  /* allow multiple sockets to use the same PORT number */
  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
    char errbuf[1024];
    ERROR("network plugin: setsockopt (reuseaddr): %s",
          sstrerror(errno, errbuf, sizeof(errbuf)));
    return (-1);
  }

  DEBUG("fd = %i; calling `bind'", fd);

  if (bind(fd, ai->ai_addr, ai->ai_addrlen) == -1) {
    char errbuf[1024];
    ERROR("bind: %s", sstrerror(errno, errbuf, sizeof(errbuf)));
    return (-1);
  }

  if (ai->ai_family == AF_INET) {
    struct sockaddr_in *addr = (struct sockaddr_in *)ai->ai_addr;
    if (IN_MULTICAST(ntohl(addr->sin_addr.s_addr))) {
#if HAVE_STRUCT_IP_MREQN_IMR_IFINDEX
      struct ip_mreqn mreq;
#else
      struct ip_mreq mreq;
#endif

      DEBUG("fd = %i; IPv4 multicast address found", fd);

      mreq.imr_multiaddr.s_addr = addr->sin_addr.s_addr;
#if HAVE_STRUCT_IP_MREQN_IMR_IFINDEX
      /* Set the interface using the interface index if
       * possible (available). Unfortunately, the struct
       * ip_mreqn is not portable. */
      mreq.imr_address.s_addr = ntohl(INADDR_ANY);
      mreq.imr_ifindex = interface_idx;
#else
      mreq.imr_interface.s_addr = ntohl(INADDR_ANY);
#endif

      if (setsockopt(fd, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop)) ==
          -1) {
        char errbuf[1024];
        ERROR("network plugin: setsockopt (multicast-loop): %s",
              sstrerror(errno, errbuf, sizeof(errbuf)));
        return (-1);
      }

      if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) ==
          -1) {
        char errbuf[1024];
        ERROR("network plugin: setsockopt (add-membership): %s",
              sstrerror(errno, errbuf, sizeof(errbuf)));
        return (-1);
      }

      return (0);
    }
  } else if (ai->ai_family == AF_INET6) {
    /* Useful example:
     * http://gsyc.escet.urjc.es/~eva/IPv6-web/examples/mcast.html */
    struct sockaddr_in6 *addr = (struct sockaddr_in6 *)ai->ai_addr;
    if (IN6_IS_ADDR_MULTICAST(&addr->sin6_addr)) {
      struct ipv6_mreq mreq;

      DEBUG("fd = %i; IPv6 multicast address found", fd);

      memcpy(&mreq.ipv6mr_multiaddr, &addr->sin6_addr, sizeof(addr->sin6_addr));

      /* http://developer.apple.com/documentation/Darwin/Reference/ManPages/man4/ip6.4.html
       * ipv6mr_interface may be set to zeroes to
       * choose the default multicast interface or to
       * the index of a particular multicast-capable
       * interface if the host is multihomed.
       * Membership is associ-associated with a
       * single interface; programs running on
       * multihomed hosts may need to join the same
       * group on more than one interface.*/
      mreq.ipv6mr_interface = interface_idx;

      if (setsockopt(fd, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, &loop,
                     sizeof(loop)) == -1) {
        char errbuf[1024];
        ERROR("network plugin: setsockopt (ipv6-multicast-loop): %s",
              sstrerror(errno, errbuf, sizeof(errbuf)));
        return (-1);
      }

      if (setsockopt(fd, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, &mreq,
                     sizeof(mreq)) == -1) {
        char errbuf[1024];
        ERROR("network plugin: setsockopt (ipv6-add-membership): %s",
              sstrerror(errno, errbuf, sizeof(errbuf)));
        return (-1);
      }

      return (0);
    }
  }

#if defined(HAVE_IF_INDEXTONAME) && HAVE_IF_INDEXTONAME &&                     \
    defined(SO_BINDTODEVICE)
  /* if a specific interface was set, bind the socket to it. But to avoid
   * possible problems with multicast routing, only do that for non-multicast
   * addresses */
  if (interface_idx != 0) {
    char interface_name[IFNAMSIZ];

    if (if_indextoname(interface_idx, interface_name) == NULL)
      return (-1);

    DEBUG("fd = %i; Binding socket to interface %s", fd, interface_name);

    if (setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, interface_name,
                   sizeof(interface_name)) == -1) {
      char errbuf[1024];
      ERROR("network plugin: setsockopt (bind-if): %s",
            sstrerror(errno, errbuf, sizeof(errbuf)));
      return (-1);
    }
  }
#endif /* HAVE_IF_INDEXTONAME && SO_BINDTODEVICE */

  return (0);
} /* int network_bind_socket */
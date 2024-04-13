static int network_set_interface(const sockent_t *se,
                                 const struct addrinfo *ai) /* {{{ */
{
  DEBUG("network plugin: network_set_interface: interface index = %i;",
        se->interface);

  assert(se->type == SOCKENT_TYPE_CLIENT);

  if (ai->ai_family == AF_INET) {
    struct sockaddr_in *addr = (struct sockaddr_in *)ai->ai_addr;

    if (IN_MULTICAST(ntohl(addr->sin_addr.s_addr))) {
#if HAVE_STRUCT_IP_MREQN_IMR_IFINDEX
      /* If possible, use the "ip_mreqn" structure which has
       * an "interface index" member. Using the interface
       * index is preferred here, because of its similarity
       * to the way IPv6 handles this. Unfortunately, it
       * appears not to be portable. */
      struct ip_mreqn mreq = {.imr_multiaddr.s_addr = addr->sin_addr.s_addr,
                              .imr_address.s_addr = ntohl(INADDR_ANY),
                              .imr_ifindex = se->interface};
#else
      struct ip_mreq mreq = {.imr_multiaddr.s_addr = addr->sin_addr.s_addr,
                             .imr_interface.s_addr = ntohl(INADDR_ANY)};
#endif

      if (setsockopt(se->data.client.fd, IPPROTO_IP, IP_MULTICAST_IF, &mreq,
                     sizeof(mreq)) != 0) {
        char errbuf[1024];
        ERROR("network plugin: setsockopt (ipv4-multicast-if): %s",
              sstrerror(errno, errbuf, sizeof(errbuf)));
        return (-1);
      }

      return (0);
    }
  } else if (ai->ai_family == AF_INET6) {
    struct sockaddr_in6 *addr = (struct sockaddr_in6 *)ai->ai_addr;

    if (IN6_IS_ADDR_MULTICAST(&addr->sin6_addr)) {
      if (setsockopt(se->data.client.fd, IPPROTO_IPV6, IPV6_MULTICAST_IF,
                     &se->interface, sizeof(se->interface)) != 0) {
        char errbuf[1024];
        ERROR("network plugin: setsockopt (ipv6-multicast-if): %s",
              sstrerror(errno, errbuf, sizeof(errbuf)));
        return (-1);
      }

      return (0);
    }
  }

  /* else: Not a multicast interface. */
  if (se->interface != 0) {
#if defined(HAVE_IF_INDEXTONAME) && HAVE_IF_INDEXTONAME &&                     \
    defined(SO_BINDTODEVICE)
    char interface_name[IFNAMSIZ];

    if (if_indextoname(se->interface, interface_name) == NULL)
      return (-1);

    DEBUG("network plugin: Binding socket to interface %s", interface_name);

    if (setsockopt(se->data.client.fd, SOL_SOCKET, SO_BINDTODEVICE,
                   interface_name, sizeof(interface_name)) == -1) {
      char errbuf[1024];
      ERROR("network plugin: setsockopt (bind-if): %s",
            sstrerror(errno, errbuf, sizeof(errbuf)));
      return (-1);
    }
/* #endif HAVE_IF_INDEXTONAME && SO_BINDTODEVICE */

#else
    WARNING("network plugin: Cannot set the interface on a unicast "
            "socket because "
#if !defined(SO_BINDTODEVICE)
            "the \"SO_BINDTODEVICE\" socket option "
#else
            "the \"if_indextoname\" function "
#endif
            "is not available on your system.");
#endif
  }

  return (0);
} /* }}} network_set_interface */
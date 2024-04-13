static int network_set_ttl (const sockent_t *se, const struct addrinfo *ai)
{
	DEBUG ("network plugin: network_set_ttl: network_config_ttl = %i;",
			network_config_ttl);

        assert (se->type == SOCKENT_TYPE_CLIENT);

	if ((network_config_ttl < 1) || (network_config_ttl > 255))
		return (-1);

	if (ai->ai_family == AF_INET)
	{
		struct sockaddr_in *addr = (struct sockaddr_in *) ai->ai_addr;
		int optname;

		if (IN_MULTICAST (ntohl (addr->sin_addr.s_addr)))
			optname = IP_MULTICAST_TTL;
		else
			optname = IP_TTL;

		if (setsockopt (se->data.client.fd, IPPROTO_IP, optname,
					&network_config_ttl,
					sizeof (network_config_ttl)) != 0)
		{
			char errbuf[1024];
			ERROR ("network plugin: setsockopt (ipv4-ttl): %s",
					sstrerror (errno, errbuf, sizeof (errbuf)));
			return (-1);
		}
	}
	else if (ai->ai_family == AF_INET6)
	{
		/* Useful example: http://gsyc.escet.urjc.es/~eva/IPv6-web/examples/mcast.html */
		struct sockaddr_in6 *addr = (struct sockaddr_in6 *) ai->ai_addr;
		int optname;

		if (IN6_IS_ADDR_MULTICAST (&addr->sin6_addr))
			optname = IPV6_MULTICAST_HOPS;
		else
			optname = IPV6_UNICAST_HOPS;

		if (setsockopt (se->data.client.fd, IPPROTO_IPV6, optname,
					&network_config_ttl,
					sizeof (network_config_ttl)) != 0)
		{
			char errbuf[1024];
			ERROR ("network plugin: setsockopt(ipv6-ttl): %s",
					sstrerror (errno, errbuf,
						sizeof (errbuf)));
			return (-1);
		}
	}

	return (0);
} /* int network_set_ttl */
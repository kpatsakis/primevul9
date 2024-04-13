sockaddr_is_local(struct sockaddr *hostaddr)
{
	switch (hostaddr->sa_family) {
	case AF_INET:
		return (ntohl(((struct sockaddr_in *)hostaddr)->
		    sin_addr.s_addr) >> 24) == IN_LOOPBACKNET;
	case AF_INET6:
		return IN6_IS_ADDR_LOOPBACK(
		    &(((struct sockaddr_in6 *)hostaddr)->sin6_addr));
	default:
		return 0;
	}
}
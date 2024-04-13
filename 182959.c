same_addrs(isc_sockaddr_t const *oldlist, isc_sockaddr_t const *newlist,
	   uint32_t count)
{
	unsigned int i;

	for (i = 0; i < count; i++)
		if (!isc_sockaddr_equal(&oldlist[i], &newlist[i]))
			return (false);
	return (true);
}
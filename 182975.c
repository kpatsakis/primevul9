same_keynames(dns_name_t * const *oldlist, dns_name_t * const *newlist,
	      uint32_t count)
{
	unsigned int i;

	if (oldlist == NULL && newlist == NULL)
		return (true);
	if (oldlist == NULL || newlist == NULL)
		return (false);

	for (i = 0; i < count; i++) {
		if (oldlist[i] == NULL && newlist[i] == NULL)
			continue;
		if (oldlist[i] == NULL || newlist[i] == NULL ||
		    !dns_name_equal(oldlist[i], newlist[i]))
			return (false);
	}
	return (true);
}
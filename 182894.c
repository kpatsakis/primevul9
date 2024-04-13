find_next_matching_tuple(dns_difftuple_t *cur) {
	dns_difftuple_t *next = cur;

	while ((next = ISC_LIST_NEXT(next, link)) != NULL) {
		if (cur->rdata.type == next->rdata.type &&
		    dns_name_equal(&cur->name, &next->name))
		{
			return (next);
		}
	}

	return (NULL);
}
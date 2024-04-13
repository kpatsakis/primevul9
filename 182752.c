message_count(dns_message_t *msg, dns_section_t section, dns_rdatatype_t type)
{
	isc_result_t result;
	dns_name_t *name;
	dns_rdataset_t *curr;
	int count = 0;

	result = dns_message_firstname(msg, section);
	while (result == ISC_R_SUCCESS) {
		name = NULL;
		dns_message_currentname(msg, section, &name);

		for (curr = ISC_LIST_TAIL(name->list); curr != NULL;
		     curr = ISC_LIST_PREV(curr, link)) {
			if (curr->type == type)
				count++;
		}
		result = dns_message_nextname(msg, section);
	}

	return (count);
}
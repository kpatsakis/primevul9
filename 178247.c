static int append_query(unsigned char *buf, unsigned int size,
				const char *query, const char *domain)
{
	unsigned char *ptr = buf;
	int len;

	debug("query %s domain %s", query, domain);

	while (query) {
		const char *tmp;

		tmp = strchr(query, '.');
		if (!tmp) {
			len = strlen(query);
			if (len == 0)
				break;
			*ptr = len;
			memcpy(ptr + 1, query, len);
			ptr += len + 1;
			break;
		}

		*ptr = tmp - query;
		memcpy(ptr + 1, query, tmp - query);
		ptr += tmp - query + 1;

		query = tmp + 1;
	}

	while (domain) {
		const char *tmp;

		tmp = strchr(domain, '.');
		if (!tmp) {
			len = strlen(domain);
			if (len == 0)
				break;
			*ptr = len;
			memcpy(ptr + 1, domain, len);
			ptr += len + 1;
			break;
		}

		*ptr = tmp - domain;
		memcpy(ptr + 1, domain, tmp - domain);
		ptr += tmp - domain + 1;

		domain = tmp + 1;
	}

	*ptr++ = 0x00;

	return ptr - buf;
}
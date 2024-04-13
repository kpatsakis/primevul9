static int strip_domains(char *name, char *answers, int maxlen)
{
	uint16_t data_len;
	int name_len = strlen(name);
	char *ptr, *start = answers, *end = answers + maxlen;

	while (maxlen > 0) {
		ptr = strstr(answers, name);
		if (ptr) {
			char *domain = ptr + name_len;

			if (*domain) {
				int domain_len = strlen(domain);

				memmove(answers + name_len,
					domain + domain_len,
					end - (domain + domain_len));

				end -= domain_len;
				maxlen -= domain_len;
			}
		}

		answers += strlen(answers) + 1;
		answers += 2 + 2 + 4;  /* skip type, class and ttl fields */

		data_len = answers[0] << 8 | answers[1];
		answers += 2; /* skip the length field */

		if (answers + data_len > end)
			return -EINVAL;

		answers += data_len;
		maxlen -= answers - ptr;
	}

	return end - start;
}
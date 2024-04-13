evbuffer_strspn(
	struct evbuffer_ptr *ptr, const char *chrset)
{
	int count = 0;
	struct evbuffer_chain *chain = ptr->internal_.chain;
	size_t i = ptr->internal_.pos_in_chain;

	if (!chain)
		return 0;

	while (1) {
		char *buffer = (char *)chain->buffer + chain->misalign;
		for (; i < chain->off; ++i) {
			const char *p = chrset;
			while (*p) {
				if (buffer[i] == *p++)
					goto next;
			}
			ptr->internal_.chain = chain;
			ptr->internal_.pos_in_chain = i;
			ptr->pos += count;
			return count;
		next:
			++count;
		}
		i = 0;

		if (! chain->next) {
			ptr->internal_.chain = chain;
			ptr->internal_.pos_in_chain = i;
			ptr->pos += count;
			return count;
		}

		chain = chain->next;
	}
}
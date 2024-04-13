evbuffer_find_eol_char(struct evbuffer_ptr *it)
{
	struct evbuffer_chain *chain = it->internal_.chain;
	size_t i = it->internal_.pos_in_chain;
	while (chain != NULL) {
		char *buffer = (char *)chain->buffer + chain->misalign;
		char *cp = find_eol_char(buffer+i, chain->off-i);
		if (cp) {
			it->internal_.chain = chain;
			it->internal_.pos_in_chain = cp - buffer;
			it->pos += (cp - buffer) - i;
			return it->pos;
		}
		it->pos += chain->off - i;
		i = 0;
		chain = chain->next;
	}

	return (-1);
}
static char *convert_label(char *start, char *end, char *ptr, char *uptr,
			int remaining_len, int *used_comp, int *used_uncomp)
{
	int pos, comp_pos;
	char name[NS_MAXLABEL];

	pos = dn_expand((u_char *)start, (u_char *)end, (u_char *)ptr,
			name, NS_MAXLABEL);
	if (pos < 0) {
		debug("uncompress error [%d/%s]", errno, strerror(errno));
		goto out;
	}

	/*
	 * We need to compress back the name so that we get back to internal
	 * label presentation.
	 */
	comp_pos = dn_comp(name, (u_char *)uptr, remaining_len, NULL, NULL);
	if (comp_pos < 0) {
		debug("compress error [%d/%s]", errno, strerror(errno));
		goto out;
	}

	*used_comp = pos;
	*used_uncomp = comp_pos;

	return ptr;

out:
	return NULL;
}
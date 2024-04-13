char_quote(struct buf *ob, struct sd_markdown *rndr, uint8_t *data, size_t offset, size_t size)
{
	size_t end, nq = 0, i, f_begin, f_end;

	/* counting the number of quotes in the delimiter */
	while (nq < size && data[nq] == '"')
		nq++;

	/* finding the next delimiter */
	i = 0;
	for (end = nq; end < size && i < nq; end++) {
		if (data[end] == '"') i++;
		else i = 0;
	}

	if (i < nq && end >= size)
		return 0; /* no matching delimiter */

	/* trimming outside whitespaces */
	f_begin = nq;
	while (f_begin < end && data[f_begin] == ' ')
		f_begin++;

	f_end = end - nq;
	while (f_end > nq && data[f_end-1] == ' ')
		f_end--;

	/* real quote */
	if (f_begin < f_end) {
		struct buf work = { data + f_begin, f_end - f_begin, 0, 0 };
		if (!rndr->cb.quote(ob, &work, rndr->opaque))
			end = 0;
	} else {
		if (!rndr->cb.quote(ob, 0, rndr->opaque))
			end = 0;
	}

	return end;
}
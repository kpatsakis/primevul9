char_codespan(struct buf *ob, struct sd_markdown *rndr, uint8_t *data, size_t offset, size_t size)
{
	size_t end, nb = 0, i, f_begin, f_end;

	/* counting the number of backticks in the delimiter */
	while (nb < size && data[nb] == '`')
		nb++;

	/* finding the next delimiter */
	i = 0;
	for (end = nb; end < size && i < nb; end++) {
		if (data[end] == '`') i++;
		else i = 0;
	}

	if (i < nb && end >= size)
		return 0; /* no matching delimiter */

	/* trimming outside whitespaces */
	f_begin = nb;
	while (f_begin < end && data[f_begin] == ' ')
		f_begin++;

	f_end = end - nb;
	while (f_end > nb && data[f_end-1] == ' ')
		f_end--;

	/* real code span */
	if (f_begin < f_end) {
		struct buf work = { data + f_begin, f_end - f_begin, 0, 0 };
		if (!rndr->cb.codespan(ob, &work, rndr->opaque))
			end = 0;
	} else {
		if (!rndr->cb.codespan(ob, 0, rndr->opaque))
			end = 0;
	}

	return end;
}
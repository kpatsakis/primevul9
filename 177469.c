parse_emph1(struct buf *ob, struct sd_markdown *rndr, uint8_t *data, size_t size, uint8_t c)
{
	size_t i = 0, len;
	struct buf *work = 0;
	int r;

	/* skipping one symbol if coming from emph3 */
	if (size > 1 && data[0] == c && data[1] == c) i = 1;

	while (i < size) {
		len = find_emph_char(data + i, size - i, c);
		if (!len) return 0;
		i += len;
		if (i >= size) return 0;

		if (data[i] == c && !_isspace(data[i - 1])) {

			if (rndr->ext_flags & MKDEXT_NO_INTRA_EMPHASIS) {
				if (i + i < size && _isalnum(data[i + 1]))
					continue;
			}

			work = rndr_newbuf(rndr, BUFFER_SPAN);
			parse_inline(work, rndr, data, i);

			if (rndr->ext_flags & MKDEXT_UNDERLINE && c == '_')
				r = rndr->cb.underline(ob, work, rndr->opaque);
			else
				r = rndr->cb.emphasis(ob, work, rndr->opaque);

			rndr_popbuf(rndr, BUFFER_SPAN);
			return r ? i + 1 : 0;
		}
	}

	return 0;
}
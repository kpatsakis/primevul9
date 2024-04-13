parse_emph2(struct buf *ob, struct sd_markdown *rndr, uint8_t *data, size_t size, uint8_t c)
{
	size_t i = 0, len;
	struct buf *work = 0;
	int r;

	while (i < size) {
		len = find_emph_char(data + i, size - i, c);
		if (!len) return 0;
		i += len;

		if (i + 1 < size && data[i] == c && data[i + 1] == c && i && !_isspace(data[i - 1])) {
			work = rndr_newbuf(rndr, BUFFER_SPAN);
			parse_inline(work, rndr, data, i);

			if (c == '~')
				r = rndr->cb.strikethrough(ob, work, rndr->opaque);
			else if (c == '=')
				r = rndr->cb.highlight(ob, work, rndr->opaque);
			else
				r = rndr->cb.double_emphasis(ob, work, rndr->opaque);

			rndr_popbuf(rndr, BUFFER_SPAN);
			return r ? i + 2 : 0;
		}
		i++;
	}
	return 0;
}
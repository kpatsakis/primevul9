parse_emph3(struct buf *ob, struct sd_markdown *rndr, uint8_t *data, size_t size, uint8_t c)
{
	size_t i = 0, len;
	int r;

	while (i < size) {
		len = find_emph_char(data + i, size - i, c);
		if (!len) return 0;
		i += len;

		/* skip whitespace preceded symbols */
		if (data[i] != c || _isspace(data[i - 1]))
			continue;

		if (i + 2 < size && data[i + 1] == c && data[i + 2] == c && rndr->cb.triple_emphasis) {
			/* triple symbol found */
			struct buf *work = rndr_newbuf(rndr, BUFFER_SPAN);

			parse_inline(work, rndr, data, i);
			r = rndr->cb.triple_emphasis(ob, work, rndr->opaque);
			rndr_popbuf(rndr, BUFFER_SPAN);
			return r ? i + 3 : 0;

		} else if (i + 1 < size && data[i + 1] == c) {
			/* double symbol found, handing over to emph1 */
			len = parse_emph1(ob, rndr, data - 2, size + 2, c);
			if (!len) return 0;
			else return len - 2;

		} else {
			/* single symbol found, handing over to emph2 */
			len = parse_emph2(ob, rndr, data - 1, size + 1, c);
			if (!len) return 0;
			else return len - 1;
		}
	}
	return 0;
}
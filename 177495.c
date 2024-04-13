char_escape(struct buf *ob, struct sd_markdown *rndr, uint8_t *data, size_t offset, size_t size)
{
	static const char *escape_chars = "\\`*_{}[]()#+-.!:|&<>^~=";
	struct buf work = { 0, 0, 0, 0 };

	if (size > 1) {
		if (strchr(escape_chars, data[1]) == NULL)
			return 0;

		if (rndr->cb.normal_text) {
			work.data = data + 1;
			work.size = 1;
			rndr->cb.normal_text(ob, &work, rndr->opaque);
		}
		else bufputc(ob, data[1]);
	} else if (size == 1) {
		bufputc(ob, data[0]);
	}

	return 2;
}
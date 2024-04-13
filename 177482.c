char_entity(struct buf *ob, struct sd_markdown *rndr, uint8_t *data, size_t offset, size_t size)
{
	size_t end = 1;
	struct buf work = { 0, 0, 0, 0 };

	if (end < size && data[end] == '#')
		end++;

	while (end < size && _isalnum(data[end]))
		end++;

	if (end < size && data[end] == ';')
		end++; /* real entity */
	else
		return 0; /* lone '&' */

	if (rndr->cb.entity) {
		work.data = data;
		work.size = end;
		rndr->cb.entity(ob, &work, rndr->opaque);
	}
	else bufput(ob, data, end);

	return end;
}
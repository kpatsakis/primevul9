char_linebreak(struct buf *ob, struct sd_markdown *rndr, uint8_t *data, size_t offset, size_t size)
{
	if (offset < 2 || data[-1] != ' ' || data[-2] != ' ')
		return 0;

	/* removing the last space from ob and rendering */
	while (ob->size && ob->data[ob->size - 1] == ' ')
		ob->size--;

	return rndr->cb.linebreak(ob, rndr->opaque) ? 1 : 0;
}
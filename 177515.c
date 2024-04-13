htmlblock_end_tag(
	const char *tag,
	size_t tag_len,
	struct sd_markdown *rndr,
	uint8_t *data,
	size_t size)
{
	size_t i, w;

	/* checking if tag is a match */
	if (tag_len + 3 >= size ||
		strncasecmp((char *)data + 2, tag, tag_len) != 0 ||
		data[tag_len + 2] != '>')
		return 0;

	/* checking white lines */
	i = tag_len + 3;
	w = 0;
	if (i < size && (w = is_empty(data + i, size - i)) == 0)
		return 0; /* non-blank after tag */
	i += w;
	w = 0;

	if (i < size)
		w = is_empty(data + i, size - i);

	return i + w;
}
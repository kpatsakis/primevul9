prefix_codefence(uint8_t *data, size_t size)
{
	size_t i = 0, n = 0;
	uint8_t c;

	/* skipping initial spaces */
	if (size < 3) return 0;
	if (data[0] == ' ') { i++;
	if (data[1] == ' ') { i++;
	if (data[2] == ' ') { i++; } } }

	/* looking at the hrule uint8_t */
	if (i + 2 >= size || !(data[i] == '~' || data[i] == '`'))
		return 0;

	c = data[i];

	/* the whole line must be the uint8_t or whitespace */
	while (i < size && data[i] == c) {
		n++; i++;
	}

	if (n < 3)
		return 0;

	return i;
}
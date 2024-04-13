static void expand_tabs(struct buf *ob, const uint8_t *line, size_t size)
{
	size_t  i = 0, tab = 0;

	while (i < size) {
		size_t org = i;

		while (i < size && line[i] != '\t') {
			i++; tab++;
		}

		if (i > org)
			bufput(ob, line + org, i - org);

		if (i >= size)
			break;

		do {
			bufputc(ob, ' '); tab++;
		} while (tab % 4);

		i++;
	}
}
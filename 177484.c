parse_table(
	struct buf *ob,
	struct sd_markdown *rndr,
	uint8_t *data,
	size_t size)
{
	size_t i;

	struct buf *header_work = 0;
	struct buf *body_work = 0;

	size_t columns;
	int *col_data = NULL;

	header_work = rndr_newbuf(rndr, BUFFER_SPAN);
	body_work = rndr_newbuf(rndr, BUFFER_BLOCK);

	i = parse_table_header(header_work, rndr, data, size, &columns, &col_data);
	if (i > 0) {

		while (i < size) {
			size_t row_start;
			int pipes = 0;

			row_start = i;

			while (i < size && data[i] != '\n')
				if (data[i++] == '|')
					pipes++;

			if (pipes == 0 || i == size) {
				i = row_start;
				break;
			}

			parse_table_row(
				body_work,
				rndr,
				data + row_start,
				i - row_start,
				columns,
				col_data, 0
			);

			i++;
		}

		if (rndr->cb.table)
			rndr->cb.table(ob, header_work, body_work, rndr->opaque);
	}

	free(col_data);
	rndr_popbuf(rndr, BUFFER_SPAN);
	rndr_popbuf(rndr, BUFFER_BLOCK);
	return i;
}
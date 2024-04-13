parse_table_row(
	struct buf *ob,
	struct sd_markdown *rndr,
	uint8_t *data,
	size_t size,
	size_t columns,
	int *col_data,
	int header_flag)
{
	size_t i = 0, col;
	struct buf *row_work = 0;

	if (!rndr->cb.table_cell || !rndr->cb.table_row)
		return;

	row_work = rndr_newbuf(rndr, BUFFER_SPAN);

	if (i < size && data[i] == '|')
		i++;

	for (col = 0; col < columns && i < size; ++col) {
		size_t cell_start, cell_end;
		struct buf *cell_work;

		cell_work = rndr_newbuf(rndr, BUFFER_SPAN);

		while (i < size && _isspace(data[i]))
			i++;

		cell_start = i;

		while (i < size && data[i] != '|')
			i++;

		cell_end = i - 1;

		while (cell_end > cell_start && _isspace(data[cell_end]))
			cell_end--;

		parse_inline(cell_work, rndr, data + cell_start, 1 + cell_end - cell_start);
		rndr->cb.table_cell(row_work, cell_work, col_data[col] | header_flag, rndr->opaque);

		rndr_popbuf(rndr, BUFFER_SPAN);
		i++;
	}

	for (; col < columns; ++col) {
		struct buf empty_cell = { 0, 0, 0, 0 };
		rndr->cb.table_cell(row_work, &empty_cell, col_data[col] | header_flag, rndr->opaque);
	}

	rndr->cb.table_row(ob, row_work, rndr->opaque);

	rndr_popbuf(rndr, BUFFER_SPAN);
}
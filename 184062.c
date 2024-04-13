smtp_tx_filtered_dataline(struct smtp_tx *tx, const char *line)
{
	if (!strcmp(line, "."))
		line = NULL;
	else {
		/* ignore data line if an error is set */
		if (tx->error)
			return 0;
	}
	io_printf(tx->filter, "%s\n", line ? line : ".");
	return line ? 0 : 1;
}
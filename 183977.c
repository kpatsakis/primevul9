header_domain_append_callback(struct smtp_tx *tx, const char *hdr,
    const char *val)
{
	size_t			i, j, linelen;
	int			escape, quote, comment, skip;
	char			buffer[APPEND_DOMAIN_BUFFER_SIZE];
	const char *line, *end;

	if (smtp_message_printf(tx, "%s:", hdr) == -1)
		return;

	j = 0;
	escape = quote = comment = skip = 0;
	memset(buffer, 0, sizeof buffer);

	for (line = val; line; line = end) {
		end = strchr(line, '\n');
		if (end) {
			linelen = end - line;
			end++;
		}
		else
			linelen = strlen(line);

		for (i = 0; i < linelen; ++i) {
			if (line[i] == '(' && !escape && !quote)
				comment++;
			if (line[i] == '"' && !escape && !comment)
				quote = !quote;
			if (line[i] == ')' && !escape && !quote && comment)
				comment--;
			if (line[i] == '\\' && !escape && !comment && !quote)
				escape = 1;
			else
				escape = 0;

			/* found a separator, buffer contains a full address */
			if (line[i] == ',' && !escape && !quote && !comment) {
				if (!skip && j + strlen(tx->session->listener->hostname) + 1 < sizeof buffer) {
					header_append_domain_buffer(buffer, tx->session->listener->hostname, sizeof buffer);
					if (tx->session->flags & SF_AUTHENTICATED &&
					    tx->session->listener->sendertable[0] &&
					    tx->session->listener->flags & F_MASQUERADE &&
					    !(strcasecmp(hdr, "From")))
						header_address_rewrite_buffer(buffer, mailaddr_to_text(&tx->evp.sender),
						    sizeof buffer);
				}
				if (smtp_message_printf(tx, "%s,", buffer) == -1)
					return;
				j = 0;
				skip = 0;
				memset(buffer, 0, sizeof buffer);
			}
			else {
				if (skip) {
					if (smtp_message_printf(tx, "%c", line[i]) == -1)
						return;
				}
				else {
					buffer[j++] = line[i];
					if (j == sizeof (buffer) - 1) {
						if (smtp_message_printf(tx, "%s", buffer) == -1)
							return;
						skip = 1;
						j = 0;
						memset(buffer, 0, sizeof buffer);
					}
				}
			}
		}
		if (skip) {
			if (smtp_message_printf(tx, "\n") == -1)
				return;
		}
		else {
			buffer[j++] = '\n';
			if (j == sizeof (buffer) - 1) {
				if (smtp_message_printf(tx, "%s", buffer) == -1)
					return;
				skip = 1;
				j = 0;
				memset(buffer, 0, sizeof buffer);
			}
		}
	}

	/* end of header, if buffer is not empty we'll process it */
	if (buffer[0]) {
		if (j + strlen(tx->session->listener->hostname) + 1 < sizeof buffer) {
			header_append_domain_buffer(buffer, tx->session->listener->hostname, sizeof buffer);
			if (tx->session->flags & SF_AUTHENTICATED &&
			    tx->session->listener->sendertable[0] &&
			    tx->session->listener->flags & F_MASQUERADE &&
			    !(strcasecmp(hdr, "From")))
				header_address_rewrite_buffer(buffer, mailaddr_to_text(&tx->evp.sender),
				    sizeof buffer);
		}
		smtp_message_printf(tx, "%s", buffer);
	}
}
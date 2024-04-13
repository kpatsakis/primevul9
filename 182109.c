struct ref **get_remote_refs(int fd_out, struct packet_reader *reader,
			     struct ref **list, int for_push,
			     const struct argv_array *ref_prefixes,
			     const struct string_list *server_options)
{
	int i;
	*list = NULL;

	if (server_supports_v2("ls-refs", 1))
		packet_write_fmt(fd_out, "command=ls-refs\n");

	if (server_supports_v2("agent", 0))
		packet_write_fmt(fd_out, "agent=%s", git_user_agent_sanitized());

	if (server_options && server_options->nr &&
	    server_supports_v2("server-option", 1))
		for (i = 0; i < server_options->nr; i++)
			packet_write_fmt(fd_out, "server-option=%s",
					 server_options->items[i].string);

	packet_delim(fd_out);
	/* When pushing we don't want to request the peeled tags */
	if (!for_push)
		packet_write_fmt(fd_out, "peel\n");
	packet_write_fmt(fd_out, "symrefs\n");
	for (i = 0; ref_prefixes && i < ref_prefixes->argc; i++) {
		packet_write_fmt(fd_out, "ref-prefix %s\n",
				 ref_prefixes->argv[i]);
	}
	packet_flush(fd_out);

	/* Process response from server */
	while (packet_reader_read(reader) == PACKET_READ_NORMAL) {
		if (!process_ref_v2(reader->line, &list))
			die(_("invalid ls-refs response: %s"), reader->line);
	}

	if (reader->status != PACKET_READ_FLUSH)
		die(_("expected flush after ref listing"));

	return list;
}
struct ref **get_remote_heads(struct packet_reader *reader,
			      struct ref **list, unsigned int flags,
			      struct oid_array *extra_have,
			      struct oid_array *shallow_points)
{
	struct ref **orig_list = list;
	int len = 0;
	enum get_remote_heads_state state = EXPECTING_FIRST_REF;
	const char *arg;

	*list = NULL;

	while (state != EXPECTING_DONE) {
		switch (packet_reader_read(reader)) {
		case PACKET_READ_EOF:
			die_initial_contact(1);
		case PACKET_READ_NORMAL:
			len = reader->pktlen;
			if (len > 4 && skip_prefix(reader->line, "ERR ", &arg))
				die(_("remote error: %s"), arg);
			break;
		case PACKET_READ_FLUSH:
			state = EXPECTING_DONE;
			break;
		case PACKET_READ_DELIM:
			die(_("invalid packet"));
		}

		switch (state) {
		case EXPECTING_FIRST_REF:
			process_capabilities(reader->line, &len);
			if (process_dummy_ref(reader->line)) {
				state = EXPECTING_SHALLOW;
				break;
			}
			state = EXPECTING_REF;
			/* fallthrough */
		case EXPECTING_REF:
			if (process_ref(reader->line, len, &list, flags, extra_have))
				break;
			state = EXPECTING_SHALLOW;
			/* fallthrough */
		case EXPECTING_SHALLOW:
			if (process_shallow(reader->line, len, shallow_points))
				break;
			die(_("protocol error: unexpected '%s'"), reader->line);
		case EXPECTING_DONE:
			break;
		}
	}

	annotate_refs_with_symref_info(*orig_list);

	return list;
}
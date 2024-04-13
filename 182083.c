enum protocol_version discover_version(struct packet_reader *reader)
{
	enum protocol_version version = protocol_unknown_version;

	/*
	 * Peek the first line of the server's response to
	 * determine the protocol version the server is speaking.
	 */
	switch (packet_reader_peek(reader)) {
	case PACKET_READ_EOF:
		die_initial_contact(0);
	case PACKET_READ_FLUSH:
	case PACKET_READ_DELIM:
		version = protocol_v0;
		break;
	case PACKET_READ_NORMAL:
		version = determine_protocol_version_client(reader->line);
		break;
	}

	switch (version) {
	case protocol_v2:
		process_capabilities_v2(reader);
		break;
	case protocol_v1:
		/* Read the peeked version line */
		packet_reader_read(reader);
		break;
	case protocol_v0:
		break;
	case protocol_unknown_version:
		BUG("unknown protocol version");
	}

	return version;
}
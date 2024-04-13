static void process_capabilities_v2(struct packet_reader *reader)
{
	while (packet_reader_read(reader) == PACKET_READ_NORMAL)
		argv_array_push(&server_capabilities_v2, reader->line);

	if (reader->status != PACKET_READ_FLUSH)
		die(_("expected flush after capabilities"));
}
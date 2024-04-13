static void remember_received_packet(struct state *st, struct msg_digest *md)
{
	if (md->encrypted) {
		/* if encrypted, duplication already done */
		if (md->raw_packet.ptr != NULL) {
			pfreeany(st->st_v1_rpacket.ptr);
			st->st_v1_rpacket = md->raw_packet;
			md->raw_packet = EMPTY_CHUNK;
		}
	} else {
		/* this may be a repeat, but it will work */
		free_chunk_content(&st->st_v1_rpacket);
		st->st_v1_rpacket = clone_bytes_as_chunk(md->packet_pbs.start,
						      pbs_room(&md->packet_pbs),
						      "raw packet");
	}
}
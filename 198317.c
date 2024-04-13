static int sctp_init_cause_fixed(struct sctp_chunk *chunk, __be16 cause_code,
				 size_t paylen)
{
	struct sctp_errhdr err;
	__u16 len;

	/* Cause code constants are now defined in network order.  */
	err.cause = cause_code;
	len = sizeof(err) + paylen;
	err.length  = htons(len);

	if (skb_tailroom(chunk->skb) < len)
		return -ENOSPC;

	chunk->subh.err_hdr = sctp_addto_chunk_fixed(chunk, sizeof(err), &err);

	return 0;
}
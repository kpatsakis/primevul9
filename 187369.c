static void encode_sequence(struct xdr_stream *xdr,
			    const struct nfs4_sequence_args *args,
			    struct compound_hdr *hdr)
{
#if defined(CONFIG_NFS_V4_1)
	struct nfs4_session *session = args->sa_session;
	struct nfs4_slot_table *tp;
	struct nfs4_slot *slot;
	__be32 *p;

	if (!session)
		return;

	tp = &session->fc_slot_table;

	WARN_ON(args->sa_slotid == NFS4_MAX_SLOT_TABLE);
	slot = tp->slots + args->sa_slotid;

	encode_op_hdr(xdr, OP_SEQUENCE, decode_sequence_maxsz, hdr);

	/*
	 * Sessionid + seqid + slotid + max slotid + cache_this
	 */
	dprintk("%s: sessionid=%u:%u:%u:%u seqid=%d slotid=%d "
		"max_slotid=%d cache_this=%d\n",
		__func__,
		((u32 *)session->sess_id.data)[0],
		((u32 *)session->sess_id.data)[1],
		((u32 *)session->sess_id.data)[2],
		((u32 *)session->sess_id.data)[3],
		slot->seq_nr, args->sa_slotid,
		tp->highest_used_slotid, args->sa_cache_this);
	p = reserve_space(xdr, NFS4_MAX_SESSIONID_LEN + 16);
	p = xdr_encode_opaque_fixed(p, session->sess_id.data, NFS4_MAX_SESSIONID_LEN);
	*p++ = cpu_to_be32(slot->seq_nr);
	*p++ = cpu_to_be32(args->sa_slotid);
	*p++ = cpu_to_be32(tp->highest_used_slotid);
	*p = cpu_to_be32(args->sa_cache_this);
#endif /* CONFIG_NFS_V4_1 */
}
static void encode_nfs4_seqid(struct xdr_stream *xdr,
		const struct nfs_seqid *seqid)
{
	encode_uint32(xdr, seqid->sequence->counter);
}
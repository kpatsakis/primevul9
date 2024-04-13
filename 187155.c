static void encode_nops(struct compound_hdr *hdr)
{
	BUG_ON(hdr->nops > NFS4_MAX_OPS);
	*hdr->nops_p = htonl(hdr->nops);
}
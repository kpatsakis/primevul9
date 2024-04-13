static void encode_exchange_id(struct xdr_stream *xdr,
			       struct nfs41_exchange_id_args *args,
			       struct compound_hdr *hdr)
{
	__be32 *p;
	char impl_name[NFS4_OPAQUE_LIMIT];
	int len = 0;

	encode_op_hdr(xdr, OP_EXCHANGE_ID, decode_exchange_id_maxsz, hdr);
	encode_nfs4_verifier(xdr, args->verifier);

	encode_string(xdr, args->id_len, args->id);

	p = reserve_space(xdr, 12);
	*p++ = cpu_to_be32(args->flags);
	*p++ = cpu_to_be32(0);	/* zero length state_protect4_a */

	if (send_implementation_id &&
	    sizeof(CONFIG_NFS_V4_1_IMPLEMENTATION_ID_DOMAIN) > 1 &&
	    sizeof(CONFIG_NFS_V4_1_IMPLEMENTATION_ID_DOMAIN)
		<= NFS4_OPAQUE_LIMIT + 1)
		len = snprintf(impl_name, sizeof(impl_name), "%s %s %s %s",
			       utsname()->sysname, utsname()->release,
			       utsname()->version, utsname()->machine);

	if (len > 0) {
		*p = cpu_to_be32(1);	/* implementation id array length=1 */

		encode_string(xdr,
			sizeof(CONFIG_NFS_V4_1_IMPLEMENTATION_ID_DOMAIN) - 1,
			CONFIG_NFS_V4_1_IMPLEMENTATION_ID_DOMAIN);
		encode_string(xdr, len, impl_name);
		/* just send zeros for nii_date - the date is in nii_name */
		p = reserve_space(xdr, 12);
		p = xdr_encode_hyper(p, 0);
		*p = cpu_to_be32(0);
	} else
		*p = cpu_to_be32(0);	/* implementation id array length=0 */
}
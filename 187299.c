static void encode_create_session(struct xdr_stream *xdr,
				  struct nfs41_create_session_args *args,
				  struct compound_hdr *hdr)
{
	__be32 *p;
	char machine_name[NFS4_MAX_MACHINE_NAME_LEN];
	uint32_t len;
	struct nfs_client *clp = args->client;
	struct nfs_net *nn = net_generic(clp->cl_net, nfs_net_id);
	u32 max_resp_sz_cached;

	/*
	 * Assumes OPEN is the biggest non-idempotent compound.
	 * 2 is the verifier.
	 */
	max_resp_sz_cached = (NFS4_dec_open_sz + RPC_REPHDRSIZE +
			      RPC_MAX_AUTH_SIZE + 2) * XDR_UNIT;

	len = scnprintf(machine_name, sizeof(machine_name), "%s",
			clp->cl_ipaddr);

	encode_op_hdr(xdr, OP_CREATE_SESSION, decode_create_session_maxsz, hdr);
	p = reserve_space(xdr, 16 + 2*28 + 20 + len + 12);
	p = xdr_encode_hyper(p, clp->cl_clientid);
	*p++ = cpu_to_be32(clp->cl_seqid);			/*Sequence id */
	*p++ = cpu_to_be32(args->flags);			/*flags */

	/* Fore Channel */
	*p++ = cpu_to_be32(0);				/* header padding size */
	*p++ = cpu_to_be32(args->fc_attrs.max_rqst_sz);	/* max req size */
	*p++ = cpu_to_be32(args->fc_attrs.max_resp_sz);	/* max resp size */
	*p++ = cpu_to_be32(max_resp_sz_cached);		/* Max resp sz cached */
	*p++ = cpu_to_be32(args->fc_attrs.max_ops);	/* max operations */
	*p++ = cpu_to_be32(args->fc_attrs.max_reqs);	/* max requests */
	*p++ = cpu_to_be32(0);				/* rdmachannel_attrs */

	/* Back Channel */
	*p++ = cpu_to_be32(0);				/* header padding size */
	*p++ = cpu_to_be32(args->bc_attrs.max_rqst_sz);	/* max req size */
	*p++ = cpu_to_be32(args->bc_attrs.max_resp_sz);	/* max resp size */
	*p++ = cpu_to_be32(args->bc_attrs.max_resp_sz_cached);	/* Max resp sz cached */
	*p++ = cpu_to_be32(args->bc_attrs.max_ops);	/* max operations */
	*p++ = cpu_to_be32(args->bc_attrs.max_reqs);	/* max requests */
	*p++ = cpu_to_be32(0);				/* rdmachannel_attrs */

	*p++ = cpu_to_be32(args->cb_program);		/* cb_program */
	*p++ = cpu_to_be32(1);
	*p++ = cpu_to_be32(RPC_AUTH_UNIX);			/* auth_sys */

	/* authsys_parms rfc1831 */
	*p++ = (__be32)nn->boot_time.tv_nsec;		/* stamp */
	p = xdr_encode_opaque(p, machine_name, len);
	*p++ = cpu_to_be32(0);				/* UID */
	*p++ = cpu_to_be32(0);				/* GID */
	*p = cpu_to_be32(0);				/* No more gids */
}
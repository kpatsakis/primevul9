static void encode_setclientid(struct xdr_stream *xdr, const struct nfs4_setclientid *setclientid, struct compound_hdr *hdr)
{
	__be32 *p;

	encode_op_hdr(xdr, OP_SETCLIENTID, decode_setclientid_maxsz, hdr);
	encode_nfs4_verifier(xdr, setclientid->sc_verifier);

	encode_string(xdr, setclientid->sc_name_len, setclientid->sc_name);
	p = reserve_space(xdr, 4);
	*p = cpu_to_be32(setclientid->sc_prog);
	encode_string(xdr, setclientid->sc_netid_len, setclientid->sc_netid);
	encode_string(xdr, setclientid->sc_uaddr_len, setclientid->sc_uaddr);
	p = reserve_space(xdr, 4);
	*p = cpu_to_be32(setclientid->sc_cb_ident);
}
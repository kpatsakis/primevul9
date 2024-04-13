nfsd4_link(struct svc_rqst *rqstp, struct nfsd4_compound_state *cstate,
	   union nfsd4_op_u *u)
{
	struct nfsd4_link *link = &u->link;
	__be32 status;

	status = nfsd_link(rqstp, &cstate->current_fh,
			   link->li_name, link->li_namelen, &cstate->save_fh);
	if (!status)
		set_change_info(&link->li_cinfo, &cstate->current_fh);
	return status;
}
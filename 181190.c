static void nfsd4_set_open_owner_reply_cache(struct nfsd4_compound_state *cstate, struct nfsd4_open *open, struct svc_fh *resfh)
{
	if (nfsd4_has_session(cstate))
		return;
	fh_copy_shallow(&open->op_openowner->oo_owner.so_replay.rp_openfh,
			&resfh->fh_handle);
}
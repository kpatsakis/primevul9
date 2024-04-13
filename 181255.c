nfsd4_getdeviceinfo(struct svc_rqst *rqstp,
		struct nfsd4_compound_state *cstate, union nfsd4_op_u *u)
{
	struct nfsd4_getdeviceinfo *gdp = &u->getdeviceinfo;
	const struct nfsd4_layout_ops *ops;
	struct nfsd4_deviceid_map *map;
	struct svc_export *exp;
	__be32 nfserr;

	dprintk("%s: layout_type %u dev_id [0x%llx:0x%x] maxcnt %u\n",
	       __func__,
	       gdp->gd_layout_type,
	       gdp->gd_devid.fsid_idx, gdp->gd_devid.generation,
	       gdp->gd_maxcount);

	map = nfsd4_find_devid_map(gdp->gd_devid.fsid_idx);
	if (!map) {
		dprintk("%s: couldn't find device ID to export mapping!\n",
			__func__);
		return nfserr_noent;
	}

	exp = rqst_exp_find(rqstp, map->fsid_type, map->fsid);
	if (IS_ERR(exp)) {
		dprintk("%s: could not find device id\n", __func__);
		return nfserr_noent;
	}

	nfserr = nfserr_layoutunavailable;
	ops = nfsd4_layout_verify(exp, gdp->gd_layout_type);
	if (!ops)
		goto out;

	nfserr = nfs_ok;
	if (gdp->gd_maxcount != 0) {
		nfserr = ops->proc_getdeviceinfo(exp->ex_path.mnt->mnt_sb,
				rqstp, cstate->session->se_client, gdp);
	}

	gdp->gd_notify_types &= ops->notify_types;
out:
	exp_put(exp);
	return nfserr;
}
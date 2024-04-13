xfs_attr_shortform_add(xfs_da_args_t *args, int forkoff)
{
	xfs_attr_shortform_t *sf;
	xfs_attr_sf_entry_t *sfe;
	int i, offset, size;
	xfs_mount_t *mp;
	xfs_inode_t *dp;
	xfs_ifork_t *ifp;

	trace_xfs_attr_sf_add(args);

	dp = args->dp;
	mp = dp->i_mount;
	dp->i_d.di_forkoff = forkoff;

	ifp = dp->i_afp;
	ASSERT(ifp->if_flags & XFS_IFINLINE);
	sf = (xfs_attr_shortform_t *)ifp->if_u1.if_data;
	sfe = &sf->list[0];
	for (i = 0; i < sf->hdr.count; sfe = XFS_ATTR_SF_NEXTENTRY(sfe), i++) {
#ifdef DEBUG
		if (sfe->namelen != args->namelen)
			continue;
		if (memcmp(args->name, sfe->nameval, args->namelen) != 0)
			continue;
		if (!xfs_attr_namesp_match(args->flags, sfe->flags))
			continue;
		ASSERT(0);
#endif
	}

	offset = (char *)sfe - (char *)sf;
	size = XFS_ATTR_SF_ENTSIZE_BYNAME(args->namelen, args->valuelen);
	xfs_idata_realloc(dp, size, XFS_ATTR_FORK);
	sf = (xfs_attr_shortform_t *)ifp->if_u1.if_data;
	sfe = (xfs_attr_sf_entry_t *)((char *)sf + offset);

	sfe->namelen = args->namelen;
	sfe->valuelen = args->valuelen;
	sfe->flags = XFS_ATTR_NSP_ARGS_TO_ONDISK(args->flags);
	memcpy(sfe->nameval, args->name, args->namelen);
	memcpy(&sfe->nameval[args->namelen], args->value, args->valuelen);
	sf->hdr.count++;
	be16_add_cpu(&sf->hdr.totsize, size);
	xfs_trans_log_inode(args->trans, dp, XFS_ILOG_CORE | XFS_ILOG_ADATA);

	xfs_sbversion_add_attr2(mp, args->trans);
}
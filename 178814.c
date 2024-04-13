xfs_bmbt_lookup_eq(
	struct xfs_btree_cur	*cur,
	struct xfs_bmbt_irec	*irec,
	int			*stat)	/* success/failure */
{
	cur->bc_rec.b = *irec;
	return xfs_btree_lookup(cur, XFS_LOOKUP_EQ, stat);
}
xfs_bmbt_lookup_first(
	struct xfs_btree_cur	*cur,
	int			*stat)	/* success/failure */
{
	cur->bc_rec.b.br_startoff = 0;
	cur->bc_rec.b.br_startblock = 0;
	cur->bc_rec.b.br_blockcount = 0;
	return xfs_btree_lookup(cur, XFS_LOOKUP_GE, stat);
}
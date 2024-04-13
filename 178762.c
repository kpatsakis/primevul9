xfs_bmbt_update(
	struct xfs_btree_cur	*cur,
	struct xfs_bmbt_irec	*irec)
{
	union xfs_btree_rec	rec;

	xfs_bmbt_disk_set_all(&rec.bmbt, irec);
	return xfs_btree_update(cur, &rec);
}
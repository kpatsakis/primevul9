xfs_bunmapi(
	xfs_trans_t		*tp,
	struct xfs_inode	*ip,
	xfs_fileoff_t		bno,
	xfs_filblks_t		len,
	int			flags,
	xfs_extnum_t		nexts,
	xfs_fsblock_t		*firstblock,
	struct xfs_defer_ops	*dfops,
	int			*done)
{
	int			error;

	error = __xfs_bunmapi(tp, ip, bno, &len, flags, nexts, firstblock,
			dfops);
	*done = (len == 0);
	return error;
}
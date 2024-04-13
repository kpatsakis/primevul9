xfs_rename(
	struct xfs_inode	*src_dp,
	struct xfs_name		*src_name,
	struct xfs_inode	*src_ip,
	struct xfs_inode	*target_dp,
	struct xfs_name		*target_name,
	struct xfs_inode	*target_ip,
	unsigned int		flags)
{
	struct xfs_mount	*mp = src_dp->i_mount;
	struct xfs_trans	*tp;
	struct xfs_inode	*wip = NULL;		/* whiteout inode */
	struct xfs_inode	*inodes[__XFS_SORT_INODES];
	int			i;
	int			num_inodes = __XFS_SORT_INODES;
	bool			new_parent = (src_dp != target_dp);
	bool			src_is_directory = S_ISDIR(VFS_I(src_ip)->i_mode);
	int			spaceres;
	int			error;

	trace_xfs_rename(src_dp, target_dp, src_name, target_name);

	if ((flags & RENAME_EXCHANGE) && !target_ip)
		return -EINVAL;

	/*
	 * If we are doing a whiteout operation, allocate the whiteout inode
	 * we will be placing at the target and ensure the type is set
	 * appropriately.
	 */
	if (flags & RENAME_WHITEOUT) {
		ASSERT(!(flags & (RENAME_NOREPLACE | RENAME_EXCHANGE)));
		error = xfs_rename_alloc_whiteout(target_dp, &wip);
		if (error)
			return error;

		/* setup target dirent info as whiteout */
		src_name->type = XFS_DIR3_FT_CHRDEV;
	}

	xfs_sort_for_rename(src_dp, target_dp, src_ip, target_ip, wip,
				inodes, &num_inodes);

	spaceres = XFS_RENAME_SPACE_RES(mp, target_name->len);
	error = xfs_trans_alloc(mp, &M_RES(mp)->tr_rename, spaceres, 0, 0, &tp);
	if (error == -ENOSPC) {
		spaceres = 0;
		error = xfs_trans_alloc(mp, &M_RES(mp)->tr_rename, 0, 0, 0,
				&tp);
	}
	if (error)
		goto out_release_wip;

	/*
	 * Attach the dquots to the inodes
	 */
	error = xfs_qm_vop_rename_dqattach(inodes);
	if (error)
		goto out_trans_cancel;

	/*
	 * Lock all the participating inodes. Depending upon whether
	 * the target_name exists in the target directory, and
	 * whether the target directory is the same as the source
	 * directory, we can lock from 2 to 4 inodes.
	 */
	xfs_lock_inodes(inodes, num_inodes, XFS_ILOCK_EXCL);

	/*
	 * Join all the inodes to the transaction. From this point on,
	 * we can rely on either trans_commit or trans_cancel to unlock
	 * them.
	 */
	xfs_trans_ijoin(tp, src_dp, XFS_ILOCK_EXCL);
	if (new_parent)
		xfs_trans_ijoin(tp, target_dp, XFS_ILOCK_EXCL);
	xfs_trans_ijoin(tp, src_ip, XFS_ILOCK_EXCL);
	if (target_ip)
		xfs_trans_ijoin(tp, target_ip, XFS_ILOCK_EXCL);
	if (wip)
		xfs_trans_ijoin(tp, wip, XFS_ILOCK_EXCL);

	/*
	 * If we are using project inheritance, we only allow renames
	 * into our tree when the project IDs are the same; else the
	 * tree quota mechanism would be circumvented.
	 */
	if (unlikely((target_dp->i_d.di_flags & XFS_DIFLAG_PROJINHERIT) &&
		     target_dp->i_d.di_projid != src_ip->i_d.di_projid)) {
		error = -EXDEV;
		goto out_trans_cancel;
	}

	/* RENAME_EXCHANGE is unique from here on. */
	if (flags & RENAME_EXCHANGE)
		return xfs_cross_rename(tp, src_dp, src_name, src_ip,
					target_dp, target_name, target_ip,
					spaceres);

	/*
	 * Check for expected errors before we dirty the transaction
	 * so we can return an error without a transaction abort.
	 *
	 * Extent count overflow check:
	 *
	 * From the perspective of src_dp, a rename operation is essentially a
	 * directory entry remove operation. Hence the only place where we check
	 * for extent count overflow for src_dp is in
	 * xfs_bmap_del_extent_real(). xfs_bmap_del_extent_real() returns
	 * -ENOSPC when it detects a possible extent count overflow and in
	 * response, the higher layers of directory handling code do the
	 * following:
	 * 1. Data/Free blocks: XFS lets these blocks linger until a
	 *    future remove operation removes them.
	 * 2. Dabtree blocks: XFS swaps the blocks with the last block in the
	 *    Leaf space and unmaps the last block.
	 *
	 * For target_dp, there are two cases depending on whether the
	 * destination directory entry exists or not.
	 *
	 * When destination directory entry does not exist (i.e. target_ip ==
	 * NULL), extent count overflow check is performed only when transaction
	 * has a non-zero sized space reservation associated with it.  With a
	 * zero-sized space reservation, XFS allows a rename operation to
	 * continue only when the directory has sufficient free space in its
	 * data/leaf/free space blocks to hold the new entry.
	 *
	 * When destination directory entry exists (i.e. target_ip != NULL), all
	 * we need to do is change the inode number associated with the already
	 * existing entry. Hence there is no need to perform an extent count
	 * overflow check.
	 */
	if (target_ip == NULL) {
		/*
		 * If there's no space reservation, check the entry will
		 * fit before actually inserting it.
		 */
		if (!spaceres) {
			error = xfs_dir_canenter(tp, target_dp, target_name);
			if (error)
				goto out_trans_cancel;
		} else {
			error = xfs_iext_count_may_overflow(target_dp,
					XFS_DATA_FORK,
					XFS_IEXT_DIR_MANIP_CNT(mp));
			if (error)
				goto out_trans_cancel;
		}
	} else {
		/*
		 * If target exists and it's a directory, check that whether
		 * it can be destroyed.
		 */
		if (S_ISDIR(VFS_I(target_ip)->i_mode) &&
		    (!xfs_dir_isempty(target_ip) ||
		     (VFS_I(target_ip)->i_nlink > 2))) {
			error = -EEXIST;
			goto out_trans_cancel;
		}
	}

	/*
	 * Lock the AGI buffers we need to handle bumping the nlink of the
	 * whiteout inode off the unlinked list and to handle dropping the
	 * nlink of the target inode.  Per locking order rules, do this in
	 * increasing AG order and before directory block allocation tries to
	 * grab AGFs because we grab AGIs before AGFs.
	 *
	 * The (vfs) caller must ensure that if src is a directory then
	 * target_ip is either null or an empty directory.
	 */
	for (i = 0; i < num_inodes && inodes[i] != NULL; i++) {
		if (inodes[i] == wip ||
		    (inodes[i] == target_ip &&
		     (VFS_I(target_ip)->i_nlink == 1 || src_is_directory))) {
			struct xfs_buf	*bp;
			xfs_agnumber_t	agno;

			agno = XFS_INO_TO_AGNO(mp, inodes[i]->i_ino);
			error = xfs_read_agi(mp, tp, agno, &bp);
			if (error)
				goto out_trans_cancel;
		}
	}

	/*
	 * Directory entry creation below may acquire the AGF. Remove
	 * the whiteout from the unlinked list first to preserve correct
	 * AGI/AGF locking order. This dirties the transaction so failures
	 * after this point will abort and log recovery will clean up the
	 * mess.
	 *
	 * For whiteouts, we need to bump the link count on the whiteout
	 * inode. After this point, we have a real link, clear the tmpfile
	 * state flag from the inode so it doesn't accidentally get misused
	 * in future.
	 */
	if (wip) {
		ASSERT(VFS_I(wip)->i_nlink == 0);
		error = xfs_iunlink_remove(tp, wip);
		if (error)
			goto out_trans_cancel;

		xfs_bumplink(tp, wip);
		VFS_I(wip)->i_state &= ~I_LINKABLE;
	}

	/*
	 * Set up the target.
	 */
	if (target_ip == NULL) {
		/*
		 * If target does not exist and the rename crosses
		 * directories, adjust the target directory link count
		 * to account for the ".." reference from the new entry.
		 */
		error = xfs_dir_createname(tp, target_dp, target_name,
					   src_ip->i_ino, spaceres);
		if (error)
			goto out_trans_cancel;

		xfs_trans_ichgtime(tp, target_dp,
					XFS_ICHGTIME_MOD | XFS_ICHGTIME_CHG);

		if (new_parent && src_is_directory) {
			xfs_bumplink(tp, target_dp);
		}
	} else { /* target_ip != NULL */
		/*
		 * Link the source inode under the target name.
		 * If the source inode is a directory and we are moving
		 * it across directories, its ".." entry will be
		 * inconsistent until we replace that down below.
		 *
		 * In case there is already an entry with the same
		 * name at the destination directory, remove it first.
		 */
		error = xfs_dir_replace(tp, target_dp, target_name,
					src_ip->i_ino, spaceres);
		if (error)
			goto out_trans_cancel;

		xfs_trans_ichgtime(tp, target_dp,
					XFS_ICHGTIME_MOD | XFS_ICHGTIME_CHG);

		/*
		 * Decrement the link count on the target since the target
		 * dir no longer points to it.
		 */
		error = xfs_droplink(tp, target_ip);
		if (error)
			goto out_trans_cancel;

		if (src_is_directory) {
			/*
			 * Drop the link from the old "." entry.
			 */
			error = xfs_droplink(tp, target_ip);
			if (error)
				goto out_trans_cancel;
		}
	} /* target_ip != NULL */

	/*
	 * Remove the source.
	 */
	if (new_parent && src_is_directory) {
		/*
		 * Rewrite the ".." entry to point to the new
		 * directory.
		 */
		error = xfs_dir_replace(tp, src_ip, &xfs_name_dotdot,
					target_dp->i_ino, spaceres);
		ASSERT(error != -EEXIST);
		if (error)
			goto out_trans_cancel;
	}

	/*
	 * We always want to hit the ctime on the source inode.
	 *
	 * This isn't strictly required by the standards since the source
	 * inode isn't really being changed, but old unix file systems did
	 * it and some incremental backup programs won't work without it.
	 */
	xfs_trans_ichgtime(tp, src_ip, XFS_ICHGTIME_CHG);
	xfs_trans_log_inode(tp, src_ip, XFS_ILOG_CORE);

	/*
	 * Adjust the link count on src_dp.  This is necessary when
	 * renaming a directory, either within one parent when
	 * the target existed, or across two parent directories.
	 */
	if (src_is_directory && (new_parent || target_ip != NULL)) {

		/*
		 * Decrement link count on src_directory since the
		 * entry that's moved no longer points to it.
		 */
		error = xfs_droplink(tp, src_dp);
		if (error)
			goto out_trans_cancel;
	}

	/*
	 * For whiteouts, we only need to update the source dirent with the
	 * inode number of the whiteout inode rather than removing it
	 * altogether.
	 */
	if (wip) {
		error = xfs_dir_replace(tp, src_dp, src_name, wip->i_ino,
					spaceres);
	} else {
		/*
		 * NOTE: We don't need to check for extent count overflow here
		 * because the dir remove name code will leave the dir block in
		 * place if the extent count would overflow.
		 */
		error = xfs_dir_removename(tp, src_dp, src_name, src_ip->i_ino,
					   spaceres);
	}

	if (error)
		goto out_trans_cancel;

	xfs_trans_ichgtime(tp, src_dp, XFS_ICHGTIME_MOD | XFS_ICHGTIME_CHG);
	xfs_trans_log_inode(tp, src_dp, XFS_ILOG_CORE);
	if (new_parent)
		xfs_trans_log_inode(tp, target_dp, XFS_ILOG_CORE);

	error = xfs_finish_rename(tp);
	if (wip)
		xfs_irele(wip);
	return error;

out_trans_cancel:
	xfs_trans_cancel(tp);
out_release_wip:
	if (wip)
		xfs_irele(wip);
	return error;
}
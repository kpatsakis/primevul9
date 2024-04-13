xfs_flush_inodes(
	struct xfs_mount	*mp)
{
	struct super_block	*sb = mp->m_super;

	if (down_read_trylock(&sb->s_umount)) {
		sync_inodes_sb(sb);
		up_read(&sb->s_umount);
	}
}
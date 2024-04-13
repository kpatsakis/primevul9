xfs_mount_alloc(
	struct super_block	*sb)
{
	struct xfs_mount	*mp;

	mp = kzalloc(sizeof(struct xfs_mount), GFP_KERNEL);
	if (!mp)
		return NULL;

	mp->m_super = sb;
	spin_lock_init(&mp->m_sb_lock);
	spin_lock_init(&mp->m_agirotor_lock);
	INIT_RADIX_TREE(&mp->m_perag_tree, GFP_ATOMIC);
	spin_lock_init(&mp->m_perag_lock);
	mutex_init(&mp->m_growlock);
	atomic_set(&mp->m_active_trans, 0);
	INIT_DELAYED_WORK(&mp->m_reclaim_work, xfs_reclaim_worker);
	INIT_DELAYED_WORK(&mp->m_eofblocks_work, xfs_eofblocks_worker);
	INIT_DELAYED_WORK(&mp->m_cowblocks_work, xfs_cowblocks_worker);
	mp->m_kobj.kobject.kset = xfs_kset;
	return mp;
}
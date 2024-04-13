xfs_init_mount_workqueues(
	struct xfs_mount	*mp)
{
	mp->m_buf_workqueue = alloc_workqueue("xfs-buf/%s",
			WQ_MEM_RECLAIM|WQ_FREEZABLE, 1, mp->m_fsname);
	if (!mp->m_buf_workqueue)
		goto out;

	mp->m_data_workqueue = alloc_workqueue("xfs-data/%s",
			WQ_MEM_RECLAIM|WQ_FREEZABLE, 0, mp->m_fsname);
	if (!mp->m_data_workqueue)
		goto out_destroy_buf;

	mp->m_unwritten_workqueue = alloc_workqueue("xfs-conv/%s",
			WQ_MEM_RECLAIM|WQ_FREEZABLE, 0, mp->m_fsname);
	if (!mp->m_unwritten_workqueue)
		goto out_destroy_data_iodone_queue;

	mp->m_cil_workqueue = alloc_workqueue("xfs-cil/%s",
			WQ_MEM_RECLAIM|WQ_FREEZABLE, 0, mp->m_fsname);
	if (!mp->m_cil_workqueue)
		goto out_destroy_unwritten;

	mp->m_reclaim_workqueue = alloc_workqueue("xfs-reclaim/%s",
			WQ_MEM_RECLAIM|WQ_FREEZABLE, 0, mp->m_fsname);
	if (!mp->m_reclaim_workqueue)
		goto out_destroy_cil;

	mp->m_log_workqueue = alloc_workqueue("xfs-log/%s",
			WQ_MEM_RECLAIM|WQ_FREEZABLE|WQ_HIGHPRI, 0,
			mp->m_fsname);
	if (!mp->m_log_workqueue)
		goto out_destroy_reclaim;

	mp->m_eofblocks_workqueue = alloc_workqueue("xfs-eofblocks/%s",
			WQ_MEM_RECLAIM|WQ_FREEZABLE, 0, mp->m_fsname);
	if (!mp->m_eofblocks_workqueue)
		goto out_destroy_log;

	mp->m_sync_workqueue = alloc_workqueue("xfs-sync/%s", WQ_FREEZABLE, 0,
					       mp->m_fsname);
	if (!mp->m_sync_workqueue)
		goto out_destroy_eofb;

	return 0;

out_destroy_eofb:
	destroy_workqueue(mp->m_eofblocks_workqueue);
out_destroy_log:
	destroy_workqueue(mp->m_log_workqueue);
out_destroy_reclaim:
	destroy_workqueue(mp->m_reclaim_workqueue);
out_destroy_cil:
	destroy_workqueue(mp->m_cil_workqueue);
out_destroy_unwritten:
	destroy_workqueue(mp->m_unwritten_workqueue);
out_destroy_data_iodone_queue:
	destroy_workqueue(mp->m_data_workqueue);
out_destroy_buf:
	destroy_workqueue(mp->m_buf_workqueue);
out:
	return -ENOMEM;
}
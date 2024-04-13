xfs_destroy_mount_workqueues(
	struct xfs_mount	*mp)
{
	destroy_workqueue(mp->m_sync_workqueue);
	destroy_workqueue(mp->m_eofblocks_workqueue);
	destroy_workqueue(mp->m_log_workqueue);
	destroy_workqueue(mp->m_reclaim_workqueue);
	destroy_workqueue(mp->m_cil_workqueue);
	destroy_workqueue(mp->m_data_workqueue);
	destroy_workqueue(mp->m_unwritten_workqueue);
	destroy_workqueue(mp->m_buf_workqueue);
}
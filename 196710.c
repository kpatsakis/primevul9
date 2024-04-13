xfs_destroy_workqueues(void)
{
	destroy_workqueue(xfs_discard_wq);
	destroy_workqueue(xfs_alloc_wq);
}
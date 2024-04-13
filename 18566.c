void ecryptfs_destroy_kthread(void)
{
	struct ecryptfs_open_req *req, *tmp;

	mutex_lock(&ecryptfs_kthread_ctl.mux);
	ecryptfs_kthread_ctl.flags |= ECRYPTFS_KTHREAD_ZOMBIE;
	list_for_each_entry_safe(req, tmp, &ecryptfs_kthread_ctl.req_list,
				 kthread_ctl_list) {
		list_del(&req->kthread_ctl_list);
		*req->lower_file = ERR_PTR(-EIO);
		complete(&req->done);
	}
	mutex_unlock(&ecryptfs_kthread_ctl.mux);
	kthread_stop(ecryptfs_kthread);
	wake_up(&ecryptfs_kthread_ctl.wait);
}
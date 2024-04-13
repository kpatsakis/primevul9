static int ecryptfs_threadfn(void *ignored)
{
	set_freezable();
	while (1)  {
		struct ecryptfs_open_req *req;

		wait_event_freezable(
			ecryptfs_kthread_ctl.wait,
			(!list_empty(&ecryptfs_kthread_ctl.req_list)
			 || kthread_should_stop()));
		mutex_lock(&ecryptfs_kthread_ctl.mux);
		if (ecryptfs_kthread_ctl.flags & ECRYPTFS_KTHREAD_ZOMBIE) {
			mutex_unlock(&ecryptfs_kthread_ctl.mux);
			goto out;
		}
		while (!list_empty(&ecryptfs_kthread_ctl.req_list)) {
			req = list_first_entry(&ecryptfs_kthread_ctl.req_list,
					       struct ecryptfs_open_req,
					       kthread_ctl_list);
			list_del(&req->kthread_ctl_list);
			*req->lower_file = dentry_open(&req->path,
				(O_RDWR | O_LARGEFILE), current_cred());
			complete(&req->done);
		}
		mutex_unlock(&ecryptfs_kthread_ctl.mux);
	}
out:
	return 0;
}
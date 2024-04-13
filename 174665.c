vcs_poll_data_free(struct vcs_poll_data *poll)
{
	unregister_vt_notifier(&poll->notifier);
	kfree(poll);
}
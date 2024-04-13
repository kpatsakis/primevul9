int unregister_oom_notifier(struct notifier_block *nb)
{
	return blocking_notifier_chain_unregister(&oom_notify_list, nb);
}
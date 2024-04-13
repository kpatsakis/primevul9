int netlink_unregister_notifier(struct notifier_block *nb)
{
	return blocking_notifier_chain_unregister(&netlink_chain, nb);
}
int fsl_hv_failover_unregister(struct notifier_block *nb)
{
	return blocking_notifier_chain_unregister(&failover_subscribers, nb);
}
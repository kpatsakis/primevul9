int fsl_hv_failover_register(struct notifier_block *nb)
{
	return blocking_notifier_chain_register(&failover_subscribers, nb);
}
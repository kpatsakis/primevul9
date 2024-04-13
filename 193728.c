static void llsec_dev_free_rcu(struct rcu_head *rcu)
{
	llsec_dev_free(container_of(rcu, struct mac802154_llsec_device, rcu));
}
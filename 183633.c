static void ip_mc_hash_add(struct in_device *in_dev,
			   struct ip_mc_list *im)
{
	struct ip_mc_list __rcu **mc_hash;
	u32 hash;

	mc_hash = rtnl_dereference(in_dev->mc_hash);
	if (mc_hash) {
		hash = ip_mc_hash(im);
		im->next_hash = mc_hash[hash];
		rcu_assign_pointer(mc_hash[hash], im);
		return;
	}

	/* do not use a hash table for small number of items */
	if (in_dev->mc_count < 4)
		return;

	mc_hash = kzalloc(sizeof(struct ip_mc_list *) << MC_HASH_SZ_LOG,
			  GFP_KERNEL);
	if (!mc_hash)
		return;

	for_each_pmc_rtnl(in_dev, im) {
		hash = ip_mc_hash(im);
		im->next_hash = mc_hash[hash];
		RCU_INIT_POINTER(mc_hash[hash], im);
	}

	rcu_assign_pointer(in_dev->mc_hash, mc_hash);
}
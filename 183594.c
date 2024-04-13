static void ip_mc_hash_remove(struct in_device *in_dev,
			      struct ip_mc_list *im)
{
	struct ip_mc_list __rcu **mc_hash = rtnl_dereference(in_dev->mc_hash);
	struct ip_mc_list *aux;

	if (!mc_hash)
		return;
	mc_hash += ip_mc_hash(im);
	while ((aux = rtnl_dereference(*mc_hash)) != im)
		mc_hash = &aux->next_hash;
	*mc_hash = im->next_hash;
}
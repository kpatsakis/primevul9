static void ip_ma_put(struct ip_mc_list *im)
{
	if (refcount_dec_and_test(&im->refcnt)) {
		in_dev_put(im->interface);
		kfree_rcu(im, rcu);
	}
}
static void nft_set_destroy(struct nft_set *set)
{
	set->ops->destroy(set);
	module_put(set->ops->owner);
	kfree(set);
}
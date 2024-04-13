static int ip4_obj_cmpfn(struct rhashtable_compare_arg *arg, const void *ptr)
{
	const struct frag_v4_compare_key *key = arg->key;
	const struct inet_frag_queue *fq = ptr;

	return !!memcmp(&fq->key, key, sizeof(*key));
}
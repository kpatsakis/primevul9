static void nfs41_init_sequence(struct nfs4_sequence_args *args,
		struct nfs4_sequence_res *res, int cache_reply)
{
	args->sa_session = NULL;
	args->sa_cache_this = 0;
	if (cache_reply)
		args->sa_cache_this = 1;
	res->sr_session = NULL;
	res->sr_slot = NULL;
}
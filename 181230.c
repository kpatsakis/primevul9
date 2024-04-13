static void nfsd4_cb_offload_release(struct nfsd4_callback *cb)
{
	struct nfsd4_copy *copy = container_of(cb, struct nfsd4_copy, cp_cb);

	nfs4_put_copy(copy);
}
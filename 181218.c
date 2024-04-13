void nfs4_put_copy(struct nfsd4_copy *copy)
{
	if (!refcount_dec_and_test(&copy->refcount))
		return;
	kfree(copy);
}
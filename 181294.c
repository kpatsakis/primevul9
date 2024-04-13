static struct nfsd4_copy *nfsd4_get_copy(struct nfs4_client *clp)
{
	struct nfsd4_copy *copy = NULL;

	spin_lock(&clp->async_lock);
	if (!list_empty(&clp->async_copies)) {
		copy = list_first_entry(&clp->async_copies, struct nfsd4_copy,
					copies);
		refcount_inc(&copy->refcount);
	}
	spin_unlock(&clp->async_lock);
	return copy;
}
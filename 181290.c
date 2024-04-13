find_async_copy(struct nfs4_client *clp, stateid_t *stateid)
{
	struct nfsd4_copy *copy;

	spin_lock(&clp->async_lock);
	list_for_each_entry(copy, &clp->async_copies, copies) {
		if (memcmp(&copy->cp_stateid, stateid, NFS4_STATEID_SIZE))
			continue;
		refcount_inc(&copy->refcount);
		spin_unlock(&clp->async_lock);
		return copy;
	}
	spin_unlock(&clp->async_lock);
	return NULL;
}
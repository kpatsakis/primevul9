check_and_set_stop_copy(struct nfsd4_copy *copy)
{
	bool value;

	spin_lock(&copy->cp_clp->async_lock);
	value = copy->stopped;
	if (!copy->stopped)
		copy->stopped = true;
	spin_unlock(&copy->cp_clp->async_lock);
	return value;
}
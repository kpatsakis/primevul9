static void ll_temac_restart_work_func(struct work_struct *work)
{
	struct temac_local *lp = container_of(work, struct temac_local,
					      restart_work.work);
	struct net_device *ndev = lp->ndev;

	ll_temac_recv(ndev);
}
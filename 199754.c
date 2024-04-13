static void ipv6_regen_rndid(unsigned long data)
{
	struct inet6_dev *idev = (struct inet6_dev *) data;
	unsigned long expires;

	rcu_read_lock_bh();
	write_lock_bh(&idev->lock);

	if (idev->dead)
		goto out;

	__ipv6_regen_rndid(idev);

	expires = jiffies +
		idev->cnf.temp_prefered_lft * HZ -
		idev->cnf.regen_max_retry * idev->cnf.dad_transmits * idev->nd_parms->retrans_time -
		idev->cnf.max_desync_factor * HZ;
	if (time_before(expires, jiffies)) {
		pr_warn("%s: too short regeneration interval; timer disabled for %s\n",
			__func__, idev->dev->name);
		goto out;
	}

	if (!mod_timer(&idev->regen_timer, expires))
		in6_dev_hold(idev);

out:
	write_unlock_bh(&idev->lock);
	rcu_read_unlock_bh();
	in6_dev_put(idev);
}
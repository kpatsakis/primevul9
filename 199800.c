static void addrconf_dad_run(struct inet6_dev *idev)
{
	struct inet6_ifaddr *ifp;

	read_lock_bh(&idev->lock);
	list_for_each_entry(ifp, &idev->addr_list, if_list) {
		spin_lock(&ifp->lock);
		if (ifp->flags & IFA_F_TENTATIVE &&
		    ifp->state == INET6_IFADDR_STATE_DAD)
			addrconf_dad_kick(ifp);
		spin_unlock(&ifp->lock);
	}
	read_unlock_bh(&idev->lock);
}
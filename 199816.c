static void update_valid_ll_addr_cnt(struct inet6_ifaddr *ifp, int count)
{
	write_lock_bh(&ifp->idev->lock);
	spin_lock(&ifp->lock);
	if (((ifp->flags & (IFA_F_PERMANENT|IFA_F_TENTATIVE|IFA_F_OPTIMISTIC|
			    IFA_F_DADFAILED)) == IFA_F_PERMANENT) &&
	    (ipv6_addr_type(&ifp->addr) & IPV6_ADDR_LINKLOCAL))
		ifp->idev->valid_ll_addr_cnt += count;
	WARN_ON(ifp->idev->valid_ll_addr_cnt < 0);
	spin_unlock(&ifp->lock);
	write_unlock_bh(&ifp->idev->lock);
}
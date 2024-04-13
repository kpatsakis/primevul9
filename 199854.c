static int ipv6_count_addresses(struct inet6_dev *idev)
{
	int cnt = 0;
	struct inet6_ifaddr *ifp;

	read_lock_bh(&idev->lock);
	list_for_each_entry(ifp, &idev->addr_list, if_list)
		cnt++;
	read_unlock_bh(&idev->lock);
	return cnt;
}
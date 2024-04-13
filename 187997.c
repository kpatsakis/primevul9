static int packet_mc_drop(struct sock *sk, struct packet_mreq_max *mreq)
{
	struct packet_mclist *ml, **mlp;

	rtnl_lock();

	for (mlp = &pkt_sk(sk)->mclist; (ml = *mlp) != NULL; mlp = &ml->next) {
		if (ml->ifindex == mreq->mr_ifindex &&
		    ml->type == mreq->mr_type &&
		    ml->alen == mreq->mr_alen &&
		    memcmp(ml->addr, mreq->mr_address, ml->alen) == 0) {
			if (--ml->count == 0) {
				struct net_device *dev;
				*mlp = ml->next;
				dev = __dev_get_by_index(sock_net(sk), ml->ifindex);
				if (dev)
					packet_dev_mc(dev, ml, -1);
				kfree(ml);
			}
			rtnl_unlock();
			return 0;
		}
	}
	rtnl_unlock();
	return -EADDRNOTAVAIL;
}
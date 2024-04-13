static void igmpv3_send_cr(struct in_device *in_dev)
{
	struct ip_mc_list *pmc, *pmc_prev, *pmc_next;
	struct sk_buff *skb = NULL;
	int type, dtype;

	rcu_read_lock();
	spin_lock_bh(&in_dev->mc_tomb_lock);

	/* deleted MCA's */
	pmc_prev = NULL;
	for (pmc = in_dev->mc_tomb; pmc; pmc = pmc_next) {
		pmc_next = pmc->next;
		if (pmc->sfmode == MCAST_INCLUDE) {
			type = IGMPV3_BLOCK_OLD_SOURCES;
			dtype = IGMPV3_BLOCK_OLD_SOURCES;
			skb = add_grec(skb, pmc, type, 1, 0);
			skb = add_grec(skb, pmc, dtype, 1, 1);
		}
		if (pmc->crcount) {
			if (pmc->sfmode == MCAST_EXCLUDE) {
				type = IGMPV3_CHANGE_TO_INCLUDE;
				skb = add_grec(skb, pmc, type, 1, 0);
			}
			pmc->crcount--;
			if (pmc->crcount == 0) {
				igmpv3_clear_zeros(&pmc->tomb);
				igmpv3_clear_zeros(&pmc->sources);
			}
		}
		if (pmc->crcount == 0 && !pmc->tomb && !pmc->sources) {
			if (pmc_prev)
				pmc_prev->next = pmc_next;
			else
				in_dev->mc_tomb = pmc_next;
			in_dev_put(pmc->interface);
			kfree_pmc(pmc);
		} else
			pmc_prev = pmc;
	}
	spin_unlock_bh(&in_dev->mc_tomb_lock);

	/* change recs */
	for_each_pmc_rcu(in_dev, pmc) {
		spin_lock_bh(&pmc->lock);
		if (pmc->sfcount[MCAST_EXCLUDE]) {
			type = IGMPV3_BLOCK_OLD_SOURCES;
			dtype = IGMPV3_ALLOW_NEW_SOURCES;
		} else {
			type = IGMPV3_ALLOW_NEW_SOURCES;
			dtype = IGMPV3_BLOCK_OLD_SOURCES;
		}
		skb = add_grec(skb, pmc, type, 0, 0);
		skb = add_grec(skb, pmc, dtype, 0, 1);	/* deleted sources */

		/* filter mode changes */
		if (pmc->crcount) {
			if (pmc->sfmode == MCAST_EXCLUDE)
				type = IGMPV3_CHANGE_TO_EXCLUDE;
			else
				type = IGMPV3_CHANGE_TO_INCLUDE;
			skb = add_grec(skb, pmc, type, 0, 0);
			pmc->crcount--;
		}
		spin_unlock_bh(&pmc->lock);
	}
	rcu_read_unlock();

	if (!skb)
		return;
	(void) igmpv3_sendpack(skb);
}
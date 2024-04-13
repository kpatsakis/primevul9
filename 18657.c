static void xfrm_policy_queue_process(struct timer_list *t)
{
	struct sk_buff *skb;
	struct sock *sk;
	struct dst_entry *dst;
	struct xfrm_policy *pol = from_timer(pol, t, polq.hold_timer);
	struct net *net = xp_net(pol);
	struct xfrm_policy_queue *pq = &pol->polq;
	struct flowi fl;
	struct sk_buff_head list;
	__u32 skb_mark;

	spin_lock(&pq->hold_queue.lock);
	skb = skb_peek(&pq->hold_queue);
	if (!skb) {
		spin_unlock(&pq->hold_queue.lock);
		goto out;
	}
	dst = skb_dst(skb);
	sk = skb->sk;

	/* Fixup the mark to support VTI. */
	skb_mark = skb->mark;
	skb->mark = pol->mark.v;
	xfrm_decode_session(skb, &fl, dst->ops->family);
	skb->mark = skb_mark;
	spin_unlock(&pq->hold_queue.lock);

	dst_hold(xfrm_dst_path(dst));
	dst = xfrm_lookup(net, xfrm_dst_path(dst), &fl, sk, XFRM_LOOKUP_QUEUE);
	if (IS_ERR(dst))
		goto purge_queue;

	if (dst->flags & DST_XFRM_QUEUE) {
		dst_release(dst);

		if (pq->timeout >= XFRM_QUEUE_TMO_MAX)
			goto purge_queue;

		pq->timeout = pq->timeout << 1;
		if (!mod_timer(&pq->hold_timer, jiffies + pq->timeout))
			xfrm_pol_hold(pol);
		goto out;
	}

	dst_release(dst);

	__skb_queue_head_init(&list);

	spin_lock(&pq->hold_queue.lock);
	pq->timeout = 0;
	skb_queue_splice_init(&pq->hold_queue, &list);
	spin_unlock(&pq->hold_queue.lock);

	while (!skb_queue_empty(&list)) {
		skb = __skb_dequeue(&list);

		/* Fixup the mark to support VTI. */
		skb_mark = skb->mark;
		skb->mark = pol->mark.v;
		xfrm_decode_session(skb, &fl, skb_dst(skb)->ops->family);
		skb->mark = skb_mark;

		dst_hold(xfrm_dst_path(skb_dst(skb)));
		dst = xfrm_lookup(net, xfrm_dst_path(skb_dst(skb)), &fl, skb->sk, 0);
		if (IS_ERR(dst)) {
			kfree_skb(skb);
			continue;
		}

		nf_reset_ct(skb);
		skb_dst_drop(skb);
		skb_dst_set(skb, dst);

		dst_output(net, skb->sk, skb);
	}

out:
	xfrm_pol_put(pol);
	return;

purge_queue:
	pq->timeout = 0;
	skb_queue_purge(&pq->hold_queue);
	xfrm_pol_put(pol);
}
static int nf_ct_frag6_queue(struct frag_queue *fq, struct sk_buff *skb,
			     const struct frag_hdr *fhdr, int nhoff)
{
	struct sk_buff *prev, *next;
	unsigned int payload_len;
	int offset, end;
	u8 ecn;

	if (fq->q.flags & INET_FRAG_COMPLETE) {
		pr_debug("Already completed\n");
		goto err;
	}

	payload_len = ntohs(ipv6_hdr(skb)->payload_len);

	offset = ntohs(fhdr->frag_off) & ~0x7;
	end = offset + (payload_len -
			((u8 *)(fhdr + 1) - (u8 *)(ipv6_hdr(skb) + 1)));

	if ((unsigned int)end > IPV6_MAXPLEN) {
		pr_debug("offset is too large.\n");
		return -EINVAL;
	}

	ecn = ip6_frag_ecn(ipv6_hdr(skb));

	if (skb->ip_summed == CHECKSUM_COMPLETE) {
		const unsigned char *nh = skb_network_header(skb);
		skb->csum = csum_sub(skb->csum,
				     csum_partial(nh, (u8 *)(fhdr + 1) - nh,
						  0));
	}

	/* Is this the final fragment? */
	if (!(fhdr->frag_off & htons(IP6_MF))) {
		/* If we already have some bits beyond end
		 * or have different end, the segment is corrupted.
		 */
		if (end < fq->q.len ||
		    ((fq->q.flags & INET_FRAG_LAST_IN) && end != fq->q.len)) {
			pr_debug("already received last fragment\n");
			goto err;
		}
		fq->q.flags |= INET_FRAG_LAST_IN;
		fq->q.len = end;
	} else {
		/* Check if the fragment is rounded to 8 bytes.
		 * Required by the RFC.
		 */
		if (end & 0x7) {
			/* RFC2460 says always send parameter problem in
			 * this case. -DaveM
			 */
			pr_debug("end of fragment not rounded to 8 bytes.\n");
			inet_frag_kill(&fq->q);
			return -EPROTO;
		}
		if (end > fq->q.len) {
			/* Some bits beyond end -> corruption. */
			if (fq->q.flags & INET_FRAG_LAST_IN) {
				pr_debug("last packet already reached.\n");
				goto err;
			}
			fq->q.len = end;
		}
	}

	if (end == offset)
		goto err;

	/* Point into the IP datagram 'data' part. */
	if (!pskb_pull(skb, (u8 *) (fhdr + 1) - skb->data)) {
		pr_debug("queue: message is too short.\n");
		goto err;
	}
	if (pskb_trim_rcsum(skb, end - offset)) {
		pr_debug("Can't trim\n");
		goto err;
	}

	/* Find out which fragments are in front and at the back of us
	 * in the chain of fragments so far.  We must know where to put
	 * this fragment, right?
	 */
	prev = fq->q.fragments_tail;
	if (!prev || prev->ip_defrag_offset < offset) {
		next = NULL;
		goto found;
	}
	prev = NULL;
	for (next = fq->q.fragments; next != NULL; next = next->next) {
		if (next->ip_defrag_offset >= offset)
			break;	/* bingo! */
		prev = next;
	}

found:
	/* RFC5722, Section 4:
	 *                                  When reassembling an IPv6 datagram, if
	 *   one or more its constituent fragments is determined to be an
	 *   overlapping fragment, the entire datagram (and any constituent
	 *   fragments, including those not yet received) MUST be silently
	 *   discarded.
	 */

	/* Check for overlap with preceding fragment. */
	if (prev &&
	    (prev->ip_defrag_offset + prev->len) > offset)
		goto discard_fq;

	/* Look for overlap with succeeding segment. */
	if (next && next->ip_defrag_offset < end)
		goto discard_fq;

	/* Note : skb->ip_defrag_offset and skb->dev share the same location */
	if (skb->dev)
		fq->iif = skb->dev->ifindex;
	/* Makes sure compiler wont do silly aliasing games */
	barrier();
	skb->ip_defrag_offset = offset;

	/* Insert this fragment in the chain of fragments. */
	skb->next = next;
	if (!next)
		fq->q.fragments_tail = skb;
	if (prev)
		prev->next = skb;
	else
		fq->q.fragments = skb;

	fq->q.stamp = skb->tstamp;
	fq->q.meat += skb->len;
	fq->ecn |= ecn;
	if (payload_len > fq->q.max_size)
		fq->q.max_size = payload_len;
	add_frag_mem_limit(fq->q.net, skb->truesize);

	/* The first fragment.
	 * nhoffset is obtained from the first fragment, of course.
	 */
	if (offset == 0) {
		fq->nhoffset = nhoff;
		fq->q.flags |= INET_FRAG_FIRST_IN;
	}

	return 0;

discard_fq:
	inet_frag_kill(&fq->q);
err:
	return -EINVAL;
}
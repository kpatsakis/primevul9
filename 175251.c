int skb_splice_bits(struct sk_buff *skb, unsigned int offset,
		    struct pipe_inode_info *pipe, unsigned int tlen,
		    unsigned int flags)
{
	struct partial_page partial[MAX_SKB_FRAGS];
	struct page *pages[MAX_SKB_FRAGS];
	struct splice_pipe_desc spd = {
		.pages = pages,
		.partial = partial,
		.nr_pages_max = MAX_SKB_FRAGS,
		.flags = flags,
		.ops = &nosteal_pipe_buf_ops,
		.spd_release = sock_spd_release,
	};
	struct sk_buff *frag_iter;
	struct sock *sk = skb->sk;
	int ret = 0;

	/*
	 * __skb_splice_bits() only fails if the output has no room left,
	 * so no point in going over the frag_list for the error case.
	 */
	if (__skb_splice_bits(skb, pipe, &offset, &tlen, &spd, sk))
		goto done;
	else if (!tlen)
		goto done;

	/*
	 * now see if we have a frag_list to map
	 */
	skb_walk_frags(skb, frag_iter) {
		if (!tlen)
			break;
		if (__skb_splice_bits(frag_iter, pipe, &offset, &tlen, &spd, sk))
			break;
	}

done:
	if (spd.nr_pages) {
		/*
		 * Drop the socket lock, otherwise we have reverse
		 * locking dependencies between sk_lock and i_mutex
		 * here as compared to sendfile(). We enter here
		 * with the socket lock held, and splice_to_pipe() will
		 * grab the pipe inode lock. For sendfile() emulation,
		 * we call into ->sendpage() with the i_mutex lock held
		 * and networking will grab the socket lock.
		 */
		release_sock(sk);
		ret = splice_to_pipe(pipe, &spd);
		lock_sock(sk);
	}

	return ret;
}
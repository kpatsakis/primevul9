static int llc_ui_sendmsg(struct socket *sock, struct msghdr *msg, size_t len)
{
	struct sock *sk = sock->sk;
	struct llc_sock *llc = llc_sk(sk);
	DECLARE_SOCKADDR(struct sockaddr_llc *, addr, msg->msg_name);
	int flags = msg->msg_flags;
	int noblock = flags & MSG_DONTWAIT;
	struct sk_buff *skb;
	size_t size = 0;
	int rc = -EINVAL, copied = 0, hdrlen;

	dprintk("%s: sending from %02X to %02X\n", __func__,
		llc->laddr.lsap, llc->daddr.lsap);
	lock_sock(sk);
	if (addr) {
		if (msg->msg_namelen < sizeof(*addr))
			goto release;
	} else {
		if (llc_ui_addr_null(&llc->addr))
			goto release;
		addr = &llc->addr;
	}
	/* must bind connection to sap if user hasn't done it. */
	if (sock_flag(sk, SOCK_ZAPPED)) {
		/* bind to sap with null dev, exclusive. */
		rc = llc_ui_autobind(sock, addr);
		if (rc)
			goto release;
	}
	hdrlen = llc->dev->hard_header_len + llc_ui_header_len(sk, addr);
	size = hdrlen + len;
	if (size > llc->dev->mtu)
		size = llc->dev->mtu;
	copied = size - hdrlen;
	release_sock(sk);
	skb = sock_alloc_send_skb(sk, size, noblock, &rc);
	lock_sock(sk);
	if (!skb)
		goto release;
	skb->dev      = llc->dev;
	skb->protocol = llc_proto_type(addr->sllc_arphrd);
	skb_reserve(skb, hdrlen);
	rc = memcpy_from_msg(skb_put(skb, copied), msg, copied);
	if (rc)
		goto out;
	if (sk->sk_type == SOCK_DGRAM || addr->sllc_ua) {
		llc_build_and_send_ui_pkt(llc->sap, skb, addr->sllc_mac,
					  addr->sllc_sap);
		goto out;
	}
	if (addr->sllc_test) {
		llc_build_and_send_test_pkt(llc->sap, skb, addr->sllc_mac,
					    addr->sllc_sap);
		goto out;
	}
	if (addr->sllc_xid) {
		llc_build_and_send_xid_pkt(llc->sap, skb, addr->sllc_mac,
					   addr->sllc_sap);
		goto out;
	}
	rc = -ENOPROTOOPT;
	if (!(sk->sk_type == SOCK_STREAM && !addr->sllc_ua))
		goto out;
	rc = llc_ui_send_data(sk, skb, noblock);
out:
	if (rc) {
		kfree_skb(skb);
release:
		dprintk("%s: failed sending from %02X to %02X: %d\n",
			__func__, llc->laddr.lsap, llc->daddr.lsap, rc);
	}
	release_sock(sk);
	return rc ? : copied;
}
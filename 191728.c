static void netlink_skb_set_owner_r(struct sk_buff *skb, struct sock *sk)
{
	WARN_ON(skb->sk != NULL);
	skb->sk = sk;
	skb->destructor = netlink_skb_destructor;
	atomic_add(skb->truesize, &sk->sk_rmem_alloc);
	sk_mem_charge(sk, skb->truesize);
}
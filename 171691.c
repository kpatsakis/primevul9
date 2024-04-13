void sk_setup_caps(struct sock *sk, struct dst_entry *dst)
{
	u32 max_segs = 1;

	sk_dst_set(sk, dst);
	sk->sk_route_caps = dst->dev->features;
	if (sk->sk_route_caps & NETIF_F_GSO)
		sk->sk_route_caps |= NETIF_F_GSO_SOFTWARE;
	sk->sk_route_caps &= ~sk->sk_route_nocaps;
	if (sk_can_gso(sk)) {
		if (dst->header_len && !xfrm_dst_offload_ok(dst)) {
			sk->sk_route_caps &= ~NETIF_F_GSO_MASK;
		} else {
			sk->sk_route_caps |= NETIF_F_SG | NETIF_F_HW_CSUM;
			sk->sk_gso_max_size = dst->dev->gso_max_size;
			max_segs = max_t(u32, dst->dev->gso_max_segs, 1);
		}
	}
	sk->sk_gso_max_segs = max_segs;
}
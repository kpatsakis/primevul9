static void bond_sk_to_flow(struct sock *sk, struct flow_keys *flow)
{
	switch (sk->sk_family) {
#if IS_ENABLED(CONFIG_IPV6)
	case AF_INET6:
		if (sk->sk_ipv6only ||
		    ipv6_addr_type(&sk->sk_v6_daddr) != IPV6_ADDR_MAPPED) {
			flow->control.addr_type = FLOW_DISSECTOR_KEY_IPV6_ADDRS;
			flow->addrs.v6addrs.src = inet6_sk(sk)->saddr;
			flow->addrs.v6addrs.dst = sk->sk_v6_daddr;
			break;
		}
		fallthrough;
#endif
	default: /* AF_INET */
		flow->control.addr_type = FLOW_DISSECTOR_KEY_IPV4_ADDRS;
		flow->addrs.v4addrs.src = inet_sk(sk)->inet_rcv_saddr;
		flow->addrs.v4addrs.dst = inet_sk(sk)->inet_daddr;
		break;
	}

	flow->ports.src = inet_sk(sk)->inet_sport;
	flow->ports.dst = inet_sk(sk)->inet_dport;
}
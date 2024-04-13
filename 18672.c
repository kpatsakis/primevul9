int __xfrm_decode_session(struct sk_buff *skb, struct flowi *fl,
			  unsigned int family, int reverse)
{
	switch (family) {
	case AF_INET:
		decode_session4(skb, fl, reverse);
		break;
#if IS_ENABLED(CONFIG_IPV6)
	case AF_INET6:
		decode_session6(skb, fl, reverse);
		break;
#endif
	default:
		return -EAFNOSUPPORT;
	}

	return security_xfrm_decode_session(skb, &fl->flowi_secid);
}
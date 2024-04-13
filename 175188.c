static size_t key_attr_size(void)
{
	return    nla_total_size(4)   /* OVS_KEY_ATTR_PRIORITY */
		+ nla_total_size(0)   /* OVS_KEY_ATTR_TUNNEL */
		  + nla_total_size(8)   /* OVS_TUNNEL_KEY_ATTR_ID */
		  + nla_total_size(4)   /* OVS_TUNNEL_KEY_ATTR_IPV4_SRC */
		  + nla_total_size(4)   /* OVS_TUNNEL_KEY_ATTR_IPV4_DST */
		  + nla_total_size(1)   /* OVS_TUNNEL_KEY_ATTR_TOS */
		  + nla_total_size(1)   /* OVS_TUNNEL_KEY_ATTR_TTL */
		  + nla_total_size(0)   /* OVS_TUNNEL_KEY_ATTR_DONT_FRAGMENT */
		  + nla_total_size(0)   /* OVS_TUNNEL_KEY_ATTR_CSUM */
		+ nla_total_size(4)   /* OVS_KEY_ATTR_IN_PORT */
		+ nla_total_size(4)   /* OVS_KEY_ATTR_SKB_MARK */
		+ nla_total_size(12)  /* OVS_KEY_ATTR_ETHERNET */
		+ nla_total_size(2)   /* OVS_KEY_ATTR_ETHERTYPE */
		+ nla_total_size(4)   /* OVS_KEY_ATTR_8021Q */
		+ nla_total_size(0)   /* OVS_KEY_ATTR_ENCAP */
		+ nla_total_size(2)   /* OVS_KEY_ATTR_ETHERTYPE */
		+ nla_total_size(40)  /* OVS_KEY_ATTR_IPV6 */
		+ nla_total_size(2)   /* OVS_KEY_ATTR_ICMPV6 */
		+ nla_total_size(28); /* OVS_KEY_ATTR_ND */
}
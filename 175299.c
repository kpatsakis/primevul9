static void __copy_skb_header(struct sk_buff *new, const struct sk_buff *old)
{
	new->tstamp		= old->tstamp;
	new->dev		= old->dev;
	new->transport_header	= old->transport_header;
	new->network_header	= old->network_header;
	new->mac_header		= old->mac_header;
	new->inner_protocol	= old->inner_protocol;
	new->inner_transport_header = old->inner_transport_header;
	new->inner_network_header = old->inner_network_header;
	new->inner_mac_header = old->inner_mac_header;
	skb_dst_copy(new, old);
	skb_copy_hash(new, old);
	new->ooo_okay		= old->ooo_okay;
	new->no_fcs		= old->no_fcs;
	new->encapsulation	= old->encapsulation;
#ifdef CONFIG_XFRM
	new->sp			= secpath_get(old->sp);
#endif
	memcpy(new->cb, old->cb, sizeof(old->cb));
	new->csum		= old->csum;
	new->local_df		= old->local_df;
	new->pkt_type		= old->pkt_type;
	new->ip_summed		= old->ip_summed;
	skb_copy_queue_mapping(new, old);
	new->priority		= old->priority;
#if IS_ENABLED(CONFIG_IP_VS)
	new->ipvs_property	= old->ipvs_property;
#endif
	new->pfmemalloc		= old->pfmemalloc;
	new->protocol		= old->protocol;
	new->mark		= old->mark;
	new->skb_iif		= old->skb_iif;
	__nf_copy(new, old);
#ifdef CONFIG_NET_SCHED
	new->tc_index		= old->tc_index;
#ifdef CONFIG_NET_CLS_ACT
	new->tc_verd		= old->tc_verd;
#endif
#endif
	new->vlan_proto		= old->vlan_proto;
	new->vlan_tci		= old->vlan_tci;

	skb_copy_secmark(new, old);

#ifdef CONFIG_NET_RX_BUSY_POLL
	new->napi_id	= old->napi_id;
#endif
}
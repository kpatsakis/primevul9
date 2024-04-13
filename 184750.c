int nfp_abm_setup_cls_block(struct net_device *netdev, struct nfp_repr *repr,
			    struct flow_block_offload *f)
{
	return flow_block_cb_setup_simple(f, &nfp_abm_block_cb_list,
					  nfp_abm_setup_tc_block_cb,
					  repr, repr, true);
}
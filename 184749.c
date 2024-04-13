static int nfp_abm_setup_tc_block_cb(enum tc_setup_type type,
				     void *type_data, void *cb_priv)
{
	struct tc_cls_u32_offload *cls_u32 = type_data;
	struct nfp_repr *repr = cb_priv;
	struct nfp_abm_link *alink;

	alink = repr->app_priv;

	if (type != TC_SETUP_CLSU32) {
		NL_SET_ERR_MSG_MOD(cls_u32->common.extack,
				   "only offload of u32 classifier supported");
		return -EOPNOTSUPP;
	}
	if (!tc_cls_can_offload_and_chain0(repr->netdev, &cls_u32->common))
		return -EOPNOTSUPP;

	if (cls_u32->common.protocol != htons(ETH_P_IP) &&
	    cls_u32->common.protocol != htons(ETH_P_IPV6)) {
		NL_SET_ERR_MSG_MOD(cls_u32->common.extack,
				   "only IP and IPv6 supported as filter protocol");
		return -EOPNOTSUPP;
	}

	switch (cls_u32->command) {
	case TC_CLSU32_NEW_KNODE:
	case TC_CLSU32_REPLACE_KNODE:
		return nfp_abm_u32_knode_replace(alink, &cls_u32->knode,
						 cls_u32->common.protocol,
						 cls_u32->common.extack);
	case TC_CLSU32_DELETE_KNODE:
		nfp_abm_u32_knode_delete(alink, &cls_u32->knode);
		return 0;
	default:
		return -EOPNOTSUPP;
	}
}
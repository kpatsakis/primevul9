nfp_abm_u32_knode_delete(struct nfp_abm_link *alink,
			 struct tc_cls_u32_knode *knode)
{
	struct nfp_abm_u32_match *iter;

	list_for_each_entry(iter, &alink->dscp_map, list)
		if (iter->handle == knode->handle) {
			list_del(&iter->list);
			kfree(iter);
			nfp_abm_update_band_map(alink);
			return;
		}
}
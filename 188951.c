static void i40e_fdir_filter_exit(struct i40e_pf *pf)
{
	struct i40e_fdir_filter *filter;
	struct i40e_flex_pit *pit_entry, *tmp;
	struct hlist_node *node2;

	hlist_for_each_entry_safe(filter, node2,
				  &pf->fdir_filter_list, fdir_node) {
		hlist_del(&filter->fdir_node);
		kfree(filter);
	}

	list_for_each_entry_safe(pit_entry, tmp, &pf->l3_flex_pit_list, list) {
		list_del(&pit_entry->list);
		kfree(pit_entry);
	}
	INIT_LIST_HEAD(&pf->l3_flex_pit_list);

	list_for_each_entry_safe(pit_entry, tmp, &pf->l4_flex_pit_list, list) {
		list_del(&pit_entry->list);
		kfree(pit_entry);
	}
	INIT_LIST_HEAD(&pf->l4_flex_pit_list);

	pf->fdir_pf_active_filters = 0;
	pf->fd_tcp4_filter_cnt = 0;
	pf->fd_udp4_filter_cnt = 0;
	pf->fd_sctp4_filter_cnt = 0;
	pf->fd_ip4_filter_cnt = 0;

	/* Reprogram the default input set for TCP/IPv4 */
	i40e_write_fd_input_set(pf, I40E_FILTER_PCTYPE_NONF_IPV4_TCP,
				I40E_L3_SRC_MASK | I40E_L3_DST_MASK |
				I40E_L4_SRC_MASK | I40E_L4_DST_MASK);

	/* Reprogram the default input set for UDP/IPv4 */
	i40e_write_fd_input_set(pf, I40E_FILTER_PCTYPE_NONF_IPV4_UDP,
				I40E_L3_SRC_MASK | I40E_L3_DST_MASK |
				I40E_L4_SRC_MASK | I40E_L4_DST_MASK);

	/* Reprogram the default input set for SCTP/IPv4 */
	i40e_write_fd_input_set(pf, I40E_FILTER_PCTYPE_NONF_IPV4_SCTP,
				I40E_L3_SRC_MASK | I40E_L3_DST_MASK |
				I40E_L4_SRC_MASK | I40E_L4_DST_MASK);

	/* Reprogram the default input set for Other/IPv4 */
	i40e_write_fd_input_set(pf, I40E_FILTER_PCTYPE_NONF_IPV4_OTHER,
				I40E_L3_SRC_MASK | I40E_L3_DST_MASK);

	i40e_write_fd_input_set(pf, I40E_FILTER_PCTYPE_FRAG_IPV4,
				I40E_L3_SRC_MASK | I40E_L3_DST_MASK);
}
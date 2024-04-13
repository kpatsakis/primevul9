void i40e_update_eth_stats(struct i40e_vsi *vsi)
{
	int stat_idx = le16_to_cpu(vsi->info.stat_counter_idx);
	struct i40e_pf *pf = vsi->back;
	struct i40e_hw *hw = &pf->hw;
	struct i40e_eth_stats *oes;
	struct i40e_eth_stats *es;     /* device's eth stats */

	es = &vsi->eth_stats;
	oes = &vsi->eth_stats_offsets;

	/* Gather up the stats that the hw collects */
	i40e_stat_update32(hw, I40E_GLV_TEPC(stat_idx),
			   vsi->stat_offsets_loaded,
			   &oes->tx_errors, &es->tx_errors);
	i40e_stat_update32(hw, I40E_GLV_RDPC(stat_idx),
			   vsi->stat_offsets_loaded,
			   &oes->rx_discards, &es->rx_discards);
	i40e_stat_update32(hw, I40E_GLV_RUPP(stat_idx),
			   vsi->stat_offsets_loaded,
			   &oes->rx_unknown_protocol, &es->rx_unknown_protocol);

	i40e_stat_update48(hw, I40E_GLV_GORCH(stat_idx),
			   I40E_GLV_GORCL(stat_idx),
			   vsi->stat_offsets_loaded,
			   &oes->rx_bytes, &es->rx_bytes);
	i40e_stat_update48(hw, I40E_GLV_UPRCH(stat_idx),
			   I40E_GLV_UPRCL(stat_idx),
			   vsi->stat_offsets_loaded,
			   &oes->rx_unicast, &es->rx_unicast);
	i40e_stat_update48(hw, I40E_GLV_MPRCH(stat_idx),
			   I40E_GLV_MPRCL(stat_idx),
			   vsi->stat_offsets_loaded,
			   &oes->rx_multicast, &es->rx_multicast);
	i40e_stat_update48(hw, I40E_GLV_BPRCH(stat_idx),
			   I40E_GLV_BPRCL(stat_idx),
			   vsi->stat_offsets_loaded,
			   &oes->rx_broadcast, &es->rx_broadcast);

	i40e_stat_update48(hw, I40E_GLV_GOTCH(stat_idx),
			   I40E_GLV_GOTCL(stat_idx),
			   vsi->stat_offsets_loaded,
			   &oes->tx_bytes, &es->tx_bytes);
	i40e_stat_update48(hw, I40E_GLV_UPTCH(stat_idx),
			   I40E_GLV_UPTCL(stat_idx),
			   vsi->stat_offsets_loaded,
			   &oes->tx_unicast, &es->tx_unicast);
	i40e_stat_update48(hw, I40E_GLV_MPTCH(stat_idx),
			   I40E_GLV_MPTCL(stat_idx),
			   vsi->stat_offsets_loaded,
			   &oes->tx_multicast, &es->tx_multicast);
	i40e_stat_update48(hw, I40E_GLV_BPTCH(stat_idx),
			   I40E_GLV_BPTCL(stat_idx),
			   vsi->stat_offsets_loaded,
			   &oes->tx_broadcast, &es->tx_broadcast);
	vsi->stat_offsets_loaded = true;
}
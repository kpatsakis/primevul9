static void i40e_update_pf_stats(struct i40e_pf *pf)
{
	struct i40e_hw_port_stats *osd = &pf->stats_offsets;
	struct i40e_hw_port_stats *nsd = &pf->stats;
	struct i40e_hw *hw = &pf->hw;
	u32 val;
	int i;

	i40e_stat_update48(hw, I40E_GLPRT_GORCH(hw->port),
			   I40E_GLPRT_GORCL(hw->port),
			   pf->stat_offsets_loaded,
			   &osd->eth.rx_bytes, &nsd->eth.rx_bytes);
	i40e_stat_update48(hw, I40E_GLPRT_GOTCH(hw->port),
			   I40E_GLPRT_GOTCL(hw->port),
			   pf->stat_offsets_loaded,
			   &osd->eth.tx_bytes, &nsd->eth.tx_bytes);
	i40e_stat_update32(hw, I40E_GLPRT_RDPC(hw->port),
			   pf->stat_offsets_loaded,
			   &osd->eth.rx_discards,
			   &nsd->eth.rx_discards);
	i40e_stat_update48(hw, I40E_GLPRT_UPRCH(hw->port),
			   I40E_GLPRT_UPRCL(hw->port),
			   pf->stat_offsets_loaded,
			   &osd->eth.rx_unicast,
			   &nsd->eth.rx_unicast);
	i40e_stat_update48(hw, I40E_GLPRT_MPRCH(hw->port),
			   I40E_GLPRT_MPRCL(hw->port),
			   pf->stat_offsets_loaded,
			   &osd->eth.rx_multicast,
			   &nsd->eth.rx_multicast);
	i40e_stat_update48(hw, I40E_GLPRT_BPRCH(hw->port),
			   I40E_GLPRT_BPRCL(hw->port),
			   pf->stat_offsets_loaded,
			   &osd->eth.rx_broadcast,
			   &nsd->eth.rx_broadcast);
	i40e_stat_update48(hw, I40E_GLPRT_UPTCH(hw->port),
			   I40E_GLPRT_UPTCL(hw->port),
			   pf->stat_offsets_loaded,
			   &osd->eth.tx_unicast,
			   &nsd->eth.tx_unicast);
	i40e_stat_update48(hw, I40E_GLPRT_MPTCH(hw->port),
			   I40E_GLPRT_MPTCL(hw->port),
			   pf->stat_offsets_loaded,
			   &osd->eth.tx_multicast,
			   &nsd->eth.tx_multicast);
	i40e_stat_update48(hw, I40E_GLPRT_BPTCH(hw->port),
			   I40E_GLPRT_BPTCL(hw->port),
			   pf->stat_offsets_loaded,
			   &osd->eth.tx_broadcast,
			   &nsd->eth.tx_broadcast);

	i40e_stat_update32(hw, I40E_GLPRT_TDOLD(hw->port),
			   pf->stat_offsets_loaded,
			   &osd->tx_dropped_link_down,
			   &nsd->tx_dropped_link_down);

	i40e_stat_update32(hw, I40E_GLPRT_CRCERRS(hw->port),
			   pf->stat_offsets_loaded,
			   &osd->crc_errors, &nsd->crc_errors);

	i40e_stat_update32(hw, I40E_GLPRT_ILLERRC(hw->port),
			   pf->stat_offsets_loaded,
			   &osd->illegal_bytes, &nsd->illegal_bytes);

	i40e_stat_update32(hw, I40E_GLPRT_MLFC(hw->port),
			   pf->stat_offsets_loaded,
			   &osd->mac_local_faults,
			   &nsd->mac_local_faults);
	i40e_stat_update32(hw, I40E_GLPRT_MRFC(hw->port),
			   pf->stat_offsets_loaded,
			   &osd->mac_remote_faults,
			   &nsd->mac_remote_faults);

	i40e_stat_update32(hw, I40E_GLPRT_RLEC(hw->port),
			   pf->stat_offsets_loaded,
			   &osd->rx_length_errors,
			   &nsd->rx_length_errors);

	i40e_stat_update32(hw, I40E_GLPRT_LXONRXC(hw->port),
			   pf->stat_offsets_loaded,
			   &osd->link_xon_rx, &nsd->link_xon_rx);
	i40e_stat_update32(hw, I40E_GLPRT_LXONTXC(hw->port),
			   pf->stat_offsets_loaded,
			   &osd->link_xon_tx, &nsd->link_xon_tx);
	i40e_stat_update32(hw, I40E_GLPRT_LXOFFRXC(hw->port),
			   pf->stat_offsets_loaded,
			   &osd->link_xoff_rx, &nsd->link_xoff_rx);
	i40e_stat_update32(hw, I40E_GLPRT_LXOFFTXC(hw->port),
			   pf->stat_offsets_loaded,
			   &osd->link_xoff_tx, &nsd->link_xoff_tx);

	for (i = 0; i < 8; i++) {
		i40e_stat_update32(hw, I40E_GLPRT_PXOFFRXC(hw->port, i),
				   pf->stat_offsets_loaded,
				   &osd->priority_xoff_rx[i],
				   &nsd->priority_xoff_rx[i]);
		i40e_stat_update32(hw, I40E_GLPRT_PXONRXC(hw->port, i),
				   pf->stat_offsets_loaded,
				   &osd->priority_xon_rx[i],
				   &nsd->priority_xon_rx[i]);
		i40e_stat_update32(hw, I40E_GLPRT_PXONTXC(hw->port, i),
				   pf->stat_offsets_loaded,
				   &osd->priority_xon_tx[i],
				   &nsd->priority_xon_tx[i]);
		i40e_stat_update32(hw, I40E_GLPRT_PXOFFTXC(hw->port, i),
				   pf->stat_offsets_loaded,
				   &osd->priority_xoff_tx[i],
				   &nsd->priority_xoff_tx[i]);
		i40e_stat_update32(hw,
				   I40E_GLPRT_RXON2OFFCNT(hw->port, i),
				   pf->stat_offsets_loaded,
				   &osd->priority_xon_2_xoff[i],
				   &nsd->priority_xon_2_xoff[i]);
	}

	i40e_stat_update48(hw, I40E_GLPRT_PRC64H(hw->port),
			   I40E_GLPRT_PRC64L(hw->port),
			   pf->stat_offsets_loaded,
			   &osd->rx_size_64, &nsd->rx_size_64);
	i40e_stat_update48(hw, I40E_GLPRT_PRC127H(hw->port),
			   I40E_GLPRT_PRC127L(hw->port),
			   pf->stat_offsets_loaded,
			   &osd->rx_size_127, &nsd->rx_size_127);
	i40e_stat_update48(hw, I40E_GLPRT_PRC255H(hw->port),
			   I40E_GLPRT_PRC255L(hw->port),
			   pf->stat_offsets_loaded,
			   &osd->rx_size_255, &nsd->rx_size_255);
	i40e_stat_update48(hw, I40E_GLPRT_PRC511H(hw->port),
			   I40E_GLPRT_PRC511L(hw->port),
			   pf->stat_offsets_loaded,
			   &osd->rx_size_511, &nsd->rx_size_511);
	i40e_stat_update48(hw, I40E_GLPRT_PRC1023H(hw->port),
			   I40E_GLPRT_PRC1023L(hw->port),
			   pf->stat_offsets_loaded,
			   &osd->rx_size_1023, &nsd->rx_size_1023);
	i40e_stat_update48(hw, I40E_GLPRT_PRC1522H(hw->port),
			   I40E_GLPRT_PRC1522L(hw->port),
			   pf->stat_offsets_loaded,
			   &osd->rx_size_1522, &nsd->rx_size_1522);
	i40e_stat_update48(hw, I40E_GLPRT_PRC9522H(hw->port),
			   I40E_GLPRT_PRC9522L(hw->port),
			   pf->stat_offsets_loaded,
			   &osd->rx_size_big, &nsd->rx_size_big);

	i40e_stat_update48(hw, I40E_GLPRT_PTC64H(hw->port),
			   I40E_GLPRT_PTC64L(hw->port),
			   pf->stat_offsets_loaded,
			   &osd->tx_size_64, &nsd->tx_size_64);
	i40e_stat_update48(hw, I40E_GLPRT_PTC127H(hw->port),
			   I40E_GLPRT_PTC127L(hw->port),
			   pf->stat_offsets_loaded,
			   &osd->tx_size_127, &nsd->tx_size_127);
	i40e_stat_update48(hw, I40E_GLPRT_PTC255H(hw->port),
			   I40E_GLPRT_PTC255L(hw->port),
			   pf->stat_offsets_loaded,
			   &osd->tx_size_255, &nsd->tx_size_255);
	i40e_stat_update48(hw, I40E_GLPRT_PTC511H(hw->port),
			   I40E_GLPRT_PTC511L(hw->port),
			   pf->stat_offsets_loaded,
			   &osd->tx_size_511, &nsd->tx_size_511);
	i40e_stat_update48(hw, I40E_GLPRT_PTC1023H(hw->port),
			   I40E_GLPRT_PTC1023L(hw->port),
			   pf->stat_offsets_loaded,
			   &osd->tx_size_1023, &nsd->tx_size_1023);
	i40e_stat_update48(hw, I40E_GLPRT_PTC1522H(hw->port),
			   I40E_GLPRT_PTC1522L(hw->port),
			   pf->stat_offsets_loaded,
			   &osd->tx_size_1522, &nsd->tx_size_1522);
	i40e_stat_update48(hw, I40E_GLPRT_PTC9522H(hw->port),
			   I40E_GLPRT_PTC9522L(hw->port),
			   pf->stat_offsets_loaded,
			   &osd->tx_size_big, &nsd->tx_size_big);

	i40e_stat_update32(hw, I40E_GLPRT_RUC(hw->port),
			   pf->stat_offsets_loaded,
			   &osd->rx_undersize, &nsd->rx_undersize);
	i40e_stat_update32(hw, I40E_GLPRT_RFC(hw->port),
			   pf->stat_offsets_loaded,
			   &osd->rx_fragments, &nsd->rx_fragments);
	i40e_stat_update32(hw, I40E_GLPRT_ROC(hw->port),
			   pf->stat_offsets_loaded,
			   &osd->rx_oversize, &nsd->rx_oversize);
	i40e_stat_update32(hw, I40E_GLPRT_RJC(hw->port),
			   pf->stat_offsets_loaded,
			   &osd->rx_jabber, &nsd->rx_jabber);

	/* FDIR stats */
	i40e_stat_update_and_clear32(hw,
			I40E_GLQF_PCNT(I40E_FD_ATR_STAT_IDX(hw->pf_id)),
			&nsd->fd_atr_match);
	i40e_stat_update_and_clear32(hw,
			I40E_GLQF_PCNT(I40E_FD_SB_STAT_IDX(hw->pf_id)),
			&nsd->fd_sb_match);
	i40e_stat_update_and_clear32(hw,
			I40E_GLQF_PCNT(I40E_FD_ATR_TUNNEL_STAT_IDX(hw->pf_id)),
			&nsd->fd_atr_tunnel_match);

	val = rd32(hw, I40E_PRTPM_EEE_STAT);
	nsd->tx_lpi_status =
		       (val & I40E_PRTPM_EEE_STAT_TX_LPI_STATUS_MASK) >>
			I40E_PRTPM_EEE_STAT_TX_LPI_STATUS_SHIFT;
	nsd->rx_lpi_status =
		       (val & I40E_PRTPM_EEE_STAT_RX_LPI_STATUS_MASK) >>
			I40E_PRTPM_EEE_STAT_RX_LPI_STATUS_SHIFT;
	i40e_stat_update32(hw, I40E_PRTPM_TLPIC,
			   pf->stat_offsets_loaded,
			   &osd->tx_lpi_count, &nsd->tx_lpi_count);
	i40e_stat_update32(hw, I40E_PRTPM_RLPIC,
			   pf->stat_offsets_loaded,
			   &osd->rx_lpi_count, &nsd->rx_lpi_count);

	if (pf->flags & I40E_FLAG_FD_SB_ENABLED &&
	    !test_bit(__I40E_FD_SB_AUTO_DISABLED, pf->state))
		nsd->fd_sb_status = true;
	else
		nsd->fd_sb_status = false;

	if (pf->flags & I40E_FLAG_FD_ATR_ENABLED &&
	    !test_bit(__I40E_FD_ATR_AUTO_DISABLED, pf->state))
		nsd->fd_atr_status = true;
	else
		nsd->fd_atr_status = false;

	pf->stat_offsets_loaded = true;
}
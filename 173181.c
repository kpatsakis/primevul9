static void bnx2x_get_ext_wwn_info(struct bnx2x *bp, int func)
{
	/* Port info */
	bp->cnic_eth_dev.fcoe_wwn_port_name_hi =
		MF_CFG_RD(bp, func_ext_config[func].fcoe_wwn_port_name_upper);
	bp->cnic_eth_dev.fcoe_wwn_port_name_lo =
		MF_CFG_RD(bp, func_ext_config[func].fcoe_wwn_port_name_lower);

	/* Node info */
	bp->cnic_eth_dev.fcoe_wwn_node_name_hi =
		MF_CFG_RD(bp, func_ext_config[func].fcoe_wwn_node_name_upper);
	bp->cnic_eth_dev.fcoe_wwn_node_name_lo =
		MF_CFG_RD(bp, func_ext_config[func].fcoe_wwn_node_name_lower);
}

static int ql_get_link_ksettings(struct net_device *ndev,
				 struct ethtool_link_ksettings *cmd)
{
	struct ql3_adapter *qdev = netdev_priv(ndev);
	u32 supported, advertising;

	supported = ql_supported_modes(qdev);

	if (test_bit(QL_LINK_OPTICAL, &qdev->flags)) {
		cmd->base.port = PORT_FIBRE;
	} else {
		cmd->base.port = PORT_TP;
		cmd->base.phy_address = qdev->PHYAddr;
	}
	advertising = ql_supported_modes(qdev);
	cmd->base.autoneg = ql_get_auto_cfg_status(qdev);
	cmd->base.speed = ql_get_speed(qdev);
	cmd->base.duplex = ql_get_full_dup(qdev);

	ethtool_convert_legacy_u32_to_link_mode(cmd->link_modes.supported,
						supported);
	ethtool_convert_legacy_u32_to_link_mode(cmd->link_modes.advertising,
						advertising);

	return 0;
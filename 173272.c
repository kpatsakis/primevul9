static void bnx2x_link_settings_requested(struct bnx2x *bp)
{
	u32 link_config, idx, cfg_size = 0;
	bp->port.advertising[0] = 0;
	bp->port.advertising[1] = 0;
	switch (bp->link_params.num_phys) {
	case 1:
	case 2:
		cfg_size = 1;
		break;
	case 3:
		cfg_size = 2;
		break;
	}
	for (idx = 0; idx < cfg_size; idx++) {
		bp->link_params.req_duplex[idx] = DUPLEX_FULL;
		link_config = bp->port.link_config[idx];
		switch (link_config & PORT_FEATURE_LINK_SPEED_MASK) {
		case PORT_FEATURE_LINK_SPEED_AUTO:
			if (bp->port.supported[idx] & SUPPORTED_Autoneg) {
				bp->link_params.req_line_speed[idx] =
					SPEED_AUTO_NEG;
				bp->port.advertising[idx] |=
					bp->port.supported[idx];
				if (bp->link_params.phy[EXT_PHY1].type ==
				    PORT_HW_CFG_XGXS_EXT_PHY_TYPE_BCM84833)
					bp->port.advertising[idx] |=
					(SUPPORTED_100baseT_Half |
					 SUPPORTED_100baseT_Full);
			} else {
				/* force 10G, no AN */
				bp->link_params.req_line_speed[idx] =
					SPEED_10000;
				bp->port.advertising[idx] |=
					(ADVERTISED_10000baseT_Full |
					 ADVERTISED_FIBRE);
				continue;
			}
			break;

		case PORT_FEATURE_LINK_SPEED_10M_FULL:
			if (bp->port.supported[idx] & SUPPORTED_10baseT_Full) {
				bp->link_params.req_line_speed[idx] =
					SPEED_10;
				bp->port.advertising[idx] |=
					(ADVERTISED_10baseT_Full |
					 ADVERTISED_TP);
			} else {
				BNX2X_ERR("NVRAM config error. Invalid link_config 0x%x  speed_cap_mask 0x%x\n",
					    link_config,
				    bp->link_params.speed_cap_mask[idx]);
				return;
			}
			break;

		case PORT_FEATURE_LINK_SPEED_10M_HALF:
			if (bp->port.supported[idx] & SUPPORTED_10baseT_Half) {
				bp->link_params.req_line_speed[idx] =
					SPEED_10;
				bp->link_params.req_duplex[idx] =
					DUPLEX_HALF;
				bp->port.advertising[idx] |=
					(ADVERTISED_10baseT_Half |
					 ADVERTISED_TP);
			} else {
				BNX2X_ERR("NVRAM config error. Invalid link_config 0x%x  speed_cap_mask 0x%x\n",
					    link_config,
					  bp->link_params.speed_cap_mask[idx]);
				return;
			}
			break;

		case PORT_FEATURE_LINK_SPEED_100M_FULL:
			if (bp->port.supported[idx] &
			    SUPPORTED_100baseT_Full) {
				bp->link_params.req_line_speed[idx] =
					SPEED_100;
				bp->port.advertising[idx] |=
					(ADVERTISED_100baseT_Full |
					 ADVERTISED_TP);
			} else {
				BNX2X_ERR("NVRAM config error. Invalid link_config 0x%x  speed_cap_mask 0x%x\n",
					    link_config,
					  bp->link_params.speed_cap_mask[idx]);
				return;
			}
			break;

		case PORT_FEATURE_LINK_SPEED_100M_HALF:
			if (bp->port.supported[idx] &
			    SUPPORTED_100baseT_Half) {
				bp->link_params.req_line_speed[idx] =
								SPEED_100;
				bp->link_params.req_duplex[idx] =
								DUPLEX_HALF;
				bp->port.advertising[idx] |=
					(ADVERTISED_100baseT_Half |
					 ADVERTISED_TP);
			} else {
				BNX2X_ERR("NVRAM config error. Invalid link_config 0x%x  speed_cap_mask 0x%x\n",
				    link_config,
				    bp->link_params.speed_cap_mask[idx]);
				return;
			}
			break;

		case PORT_FEATURE_LINK_SPEED_1G:
			if (bp->port.supported[idx] &
			    SUPPORTED_1000baseT_Full) {
				bp->link_params.req_line_speed[idx] =
					SPEED_1000;
				bp->port.advertising[idx] |=
					(ADVERTISED_1000baseT_Full |
					 ADVERTISED_TP);
			} else if (bp->port.supported[idx] &
				   SUPPORTED_1000baseKX_Full) {
				bp->link_params.req_line_speed[idx] =
					SPEED_1000;
				bp->port.advertising[idx] |=
					ADVERTISED_1000baseKX_Full;
			} else {
				BNX2X_ERR("NVRAM config error. Invalid link_config 0x%x  speed_cap_mask 0x%x\n",
				    link_config,
				    bp->link_params.speed_cap_mask[idx]);
				return;
			}
			break;

		case PORT_FEATURE_LINK_SPEED_2_5G:
			if (bp->port.supported[idx] &
			    SUPPORTED_2500baseX_Full) {
				bp->link_params.req_line_speed[idx] =
					SPEED_2500;
				bp->port.advertising[idx] |=
					(ADVERTISED_2500baseX_Full |
						ADVERTISED_TP);
			} else {
				BNX2X_ERR("NVRAM config error. Invalid link_config 0x%x  speed_cap_mask 0x%x\n",
				    link_config,
				    bp->link_params.speed_cap_mask[idx]);
				return;
			}
			break;

		case PORT_FEATURE_LINK_SPEED_10G_CX4:
			if (bp->port.supported[idx] &
			    SUPPORTED_10000baseT_Full) {
				bp->link_params.req_line_speed[idx] =
					SPEED_10000;
				bp->port.advertising[idx] |=
					(ADVERTISED_10000baseT_Full |
						ADVERTISED_FIBRE);
			} else if (bp->port.supported[idx] &
				   SUPPORTED_10000baseKR_Full) {
				bp->link_params.req_line_speed[idx] =
					SPEED_10000;
				bp->port.advertising[idx] |=
					(ADVERTISED_10000baseKR_Full |
						ADVERTISED_FIBRE);
			} else {
				BNX2X_ERR("NVRAM config error. Invalid link_config 0x%x  speed_cap_mask 0x%x\n",
				    link_config,
				    bp->link_params.speed_cap_mask[idx]);
				return;
			}
			break;
		case PORT_FEATURE_LINK_SPEED_20G:
			bp->link_params.req_line_speed[idx] = SPEED_20000;

			break;
		default:
			BNX2X_ERR("NVRAM config error. BAD link speed link_config 0x%x\n",
				  link_config);
				bp->link_params.req_line_speed[idx] =
							SPEED_AUTO_NEG;
				bp->port.advertising[idx] =
						bp->port.supported[idx];
			break;
		}

		bp->link_params.req_flow_ctrl[idx] = (link_config &
					 PORT_FEATURE_FLOW_CONTROL_MASK);
		if (bp->link_params.req_flow_ctrl[idx] ==
		    BNX2X_FLOW_CTRL_AUTO) {
			if (!(bp->port.supported[idx] & SUPPORTED_Autoneg))
				bp->link_params.req_flow_ctrl[idx] =
							BNX2X_FLOW_CTRL_NONE;
			else
				bnx2x_set_requested_fc(bp);
		}

		BNX2X_DEV_INFO("req_line_speed %d  req_duplex %d req_flow_ctrl 0x%x advertising 0x%x\n",
			       bp->link_params.req_line_speed[idx],
			       bp->link_params.req_duplex[idx],
			       bp->link_params.req_flow_ctrl[idx],
			       bp->port.advertising[idx]);
	}
}
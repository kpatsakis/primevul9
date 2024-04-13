static int bond_check_params(struct bond_params *params)
{
	int arp_validate_value, fail_over_mac_value, primary_reselect_value, i;
	struct bond_opt_value newval;
	const struct bond_opt_value *valptr;
	int arp_all_targets_value = 0;
	u16 ad_actor_sys_prio = 0;
	u16 ad_user_port_key = 0;
	__be32 arp_target[BOND_MAX_ARP_TARGETS] = { 0 };
	int arp_ip_count;
	int bond_mode	= BOND_MODE_ROUNDROBIN;
	int xmit_hashtype = BOND_XMIT_POLICY_LAYER2;
	int lacp_fast = 0;
	int tlb_dynamic_lb;

	/* Convert string parameters. */
	if (mode) {
		bond_opt_initstr(&newval, mode);
		valptr = bond_opt_parse(bond_opt_get(BOND_OPT_MODE), &newval);
		if (!valptr) {
			pr_err("Error: Invalid bonding mode \"%s\"\n", mode);
			return -EINVAL;
		}
		bond_mode = valptr->value;
	}

	if (xmit_hash_policy) {
		if (bond_mode == BOND_MODE_ROUNDROBIN ||
		    bond_mode == BOND_MODE_ACTIVEBACKUP ||
		    bond_mode == BOND_MODE_BROADCAST) {
			pr_info("xmit_hash_policy param is irrelevant in mode %s\n",
				bond_mode_name(bond_mode));
		} else {
			bond_opt_initstr(&newval, xmit_hash_policy);
			valptr = bond_opt_parse(bond_opt_get(BOND_OPT_XMIT_HASH),
						&newval);
			if (!valptr) {
				pr_err("Error: Invalid xmit_hash_policy \"%s\"\n",
				       xmit_hash_policy);
				return -EINVAL;
			}
			xmit_hashtype = valptr->value;
		}
	}

	if (lacp_rate) {
		if (bond_mode != BOND_MODE_8023AD) {
			pr_info("lacp_rate param is irrelevant in mode %s\n",
				bond_mode_name(bond_mode));
		} else {
			bond_opt_initstr(&newval, lacp_rate);
			valptr = bond_opt_parse(bond_opt_get(BOND_OPT_LACP_RATE),
						&newval);
			if (!valptr) {
				pr_err("Error: Invalid lacp rate \"%s\"\n",
				       lacp_rate);
				return -EINVAL;
			}
			lacp_fast = valptr->value;
		}
	}

	if (ad_select) {
		bond_opt_initstr(&newval, ad_select);
		valptr = bond_opt_parse(bond_opt_get(BOND_OPT_AD_SELECT),
					&newval);
		if (!valptr) {
			pr_err("Error: Invalid ad_select \"%s\"\n", ad_select);
			return -EINVAL;
		}
		params->ad_select = valptr->value;
		if (bond_mode != BOND_MODE_8023AD)
			pr_warn("ad_select param only affects 802.3ad mode\n");
	} else {
		params->ad_select = BOND_AD_STABLE;
	}

	if (max_bonds < 0) {
		pr_warn("Warning: max_bonds (%d) not in range %d-%d, so it was reset to BOND_DEFAULT_MAX_BONDS (%d)\n",
			max_bonds, 0, INT_MAX, BOND_DEFAULT_MAX_BONDS);
		max_bonds = BOND_DEFAULT_MAX_BONDS;
	}

	if (miimon < 0) {
		pr_warn("Warning: miimon module parameter (%d), not in range 0-%d, so it was reset to 0\n",
			miimon, INT_MAX);
		miimon = 0;
	}

	if (updelay < 0) {
		pr_warn("Warning: updelay module parameter (%d), not in range 0-%d, so it was reset to 0\n",
			updelay, INT_MAX);
		updelay = 0;
	}

	if (downdelay < 0) {
		pr_warn("Warning: downdelay module parameter (%d), not in range 0-%d, so it was reset to 0\n",
			downdelay, INT_MAX);
		downdelay = 0;
	}

	if ((use_carrier != 0) && (use_carrier != 1)) {
		pr_warn("Warning: use_carrier module parameter (%d), not of valid value (0/1), so it was set to 1\n",
			use_carrier);
		use_carrier = 1;
	}

	if (num_peer_notif < 0 || num_peer_notif > 255) {
		pr_warn("Warning: num_grat_arp/num_unsol_na (%d) not in range 0-255 so it was reset to 1\n",
			num_peer_notif);
		num_peer_notif = 1;
	}

	/* reset values for 802.3ad/TLB/ALB */
	if (!bond_mode_uses_arp(bond_mode)) {
		if (!miimon) {
			pr_warn("Warning: miimon must be specified, otherwise bonding will not detect link failure, speed and duplex which are essential for 802.3ad operation\n");
			pr_warn("Forcing miimon to 100msec\n");
			miimon = BOND_DEFAULT_MIIMON;
		}
	}

	if (tx_queues < 1 || tx_queues > 255) {
		pr_warn("Warning: tx_queues (%d) should be between 1 and 255, resetting to %d\n",
			tx_queues, BOND_DEFAULT_TX_QUEUES);
		tx_queues = BOND_DEFAULT_TX_QUEUES;
	}

	if ((all_slaves_active != 0) && (all_slaves_active != 1)) {
		pr_warn("Warning: all_slaves_active module parameter (%d), not of valid value (0/1), so it was set to 0\n",
			all_slaves_active);
		all_slaves_active = 0;
	}

	if (resend_igmp < 0 || resend_igmp > 255) {
		pr_warn("Warning: resend_igmp (%d) should be between 0 and 255, resetting to %d\n",
			resend_igmp, BOND_DEFAULT_RESEND_IGMP);
		resend_igmp = BOND_DEFAULT_RESEND_IGMP;
	}

	bond_opt_initval(&newval, packets_per_slave);
	if (!bond_opt_parse(bond_opt_get(BOND_OPT_PACKETS_PER_SLAVE), &newval)) {
		pr_warn("Warning: packets_per_slave (%d) should be between 0 and %u resetting to 1\n",
			packets_per_slave, USHRT_MAX);
		packets_per_slave = 1;
	}

	if (bond_mode == BOND_MODE_ALB) {
		pr_notice("In ALB mode you might experience client disconnections upon reconnection of a link if the bonding module updelay parameter (%d msec) is incompatible with the forwarding delay time of the switch\n",
			  updelay);
	}

	if (!miimon) {
		if (updelay || downdelay) {
			/* just warn the user the up/down delay will have
			 * no effect since miimon is zero...
			 */
			pr_warn("Warning: miimon module parameter not set and updelay (%d) or downdelay (%d) module parameter is set; updelay and downdelay have no effect unless miimon is set\n",
				updelay, downdelay);
		}
	} else {
		/* don't allow arp monitoring */
		if (arp_interval) {
			pr_warn("Warning: miimon (%d) and arp_interval (%d) can't be used simultaneously, disabling ARP monitoring\n",
				miimon, arp_interval);
			arp_interval = 0;
		}

		if ((updelay % miimon) != 0) {
			pr_warn("Warning: updelay (%d) is not a multiple of miimon (%d), updelay rounded to %d ms\n",
				updelay, miimon, (updelay / miimon) * miimon);
		}

		updelay /= miimon;

		if ((downdelay % miimon) != 0) {
			pr_warn("Warning: downdelay (%d) is not a multiple of miimon (%d), downdelay rounded to %d ms\n",
				downdelay, miimon,
				(downdelay / miimon) * miimon);
		}

		downdelay /= miimon;
	}

	if (arp_interval < 0) {
		pr_warn("Warning: arp_interval module parameter (%d), not in range 0-%d, so it was reset to 0\n",
			arp_interval, INT_MAX);
		arp_interval = 0;
	}

	for (arp_ip_count = 0, i = 0;
	     (arp_ip_count < BOND_MAX_ARP_TARGETS) && arp_ip_target[i]; i++) {
		__be32 ip;

		/* not a complete check, but good enough to catch mistakes */
		if (!in4_pton(arp_ip_target[i], -1, (u8 *)&ip, -1, NULL) ||
		    !bond_is_ip_target_ok(ip)) {
			pr_warn("Warning: bad arp_ip_target module parameter (%s), ARP monitoring will not be performed\n",
				arp_ip_target[i]);
			arp_interval = 0;
		} else {
			if (bond_get_targets_ip(arp_target, ip) == -1)
				arp_target[arp_ip_count++] = ip;
			else
				pr_warn("Warning: duplicate address %pI4 in arp_ip_target, skipping\n",
					&ip);
		}
	}

	if (arp_interval && !arp_ip_count) {
		/* don't allow arping if no arp_ip_target given... */
		pr_warn("Warning: arp_interval module parameter (%d) specified without providing an arp_ip_target parameter, arp_interval was reset to 0\n",
			arp_interval);
		arp_interval = 0;
	}

	if (arp_validate) {
		if (!arp_interval) {
			pr_err("arp_validate requires arp_interval\n");
			return -EINVAL;
		}

		bond_opt_initstr(&newval, arp_validate);
		valptr = bond_opt_parse(bond_opt_get(BOND_OPT_ARP_VALIDATE),
					&newval);
		if (!valptr) {
			pr_err("Error: invalid arp_validate \"%s\"\n",
			       arp_validate);
			return -EINVAL;
		}
		arp_validate_value = valptr->value;
	} else {
		arp_validate_value = 0;
	}

	if (arp_all_targets) {
		bond_opt_initstr(&newval, arp_all_targets);
		valptr = bond_opt_parse(bond_opt_get(BOND_OPT_ARP_ALL_TARGETS),
					&newval);
		if (!valptr) {
			pr_err("Error: invalid arp_all_targets_value \"%s\"\n",
			       arp_all_targets);
			arp_all_targets_value = 0;
		} else {
			arp_all_targets_value = valptr->value;
		}
	}

	if (miimon) {
		pr_info("MII link monitoring set to %d ms\n", miimon);
	} else if (arp_interval) {
		valptr = bond_opt_get_val(BOND_OPT_ARP_VALIDATE,
					  arp_validate_value);
		pr_info("ARP monitoring set to %d ms, validate %s, with %d target(s):",
			arp_interval, valptr->string, arp_ip_count);

		for (i = 0; i < arp_ip_count; i++)
			pr_cont(" %s", arp_ip_target[i]);

		pr_cont("\n");

	} else if (max_bonds) {
		/* miimon and arp_interval not set, we need one so things
		 * work as expected, see bonding.txt for details
		 */
		pr_debug("Warning: either miimon or arp_interval and arp_ip_target module parameters must be specified, otherwise bonding will not detect link failures! see bonding.txt for details\n");
	}

	if (primary && !bond_mode_uses_primary(bond_mode)) {
		/* currently, using a primary only makes sense
		 * in active backup, TLB or ALB modes
		 */
		pr_warn("Warning: %s primary device specified but has no effect in %s mode\n",
			primary, bond_mode_name(bond_mode));
		primary = NULL;
	}

	if (primary && primary_reselect) {
		bond_opt_initstr(&newval, primary_reselect);
		valptr = bond_opt_parse(bond_opt_get(BOND_OPT_PRIMARY_RESELECT),
					&newval);
		if (!valptr) {
			pr_err("Error: Invalid primary_reselect \"%s\"\n",
			       primary_reselect);
			return -EINVAL;
		}
		primary_reselect_value = valptr->value;
	} else {
		primary_reselect_value = BOND_PRI_RESELECT_ALWAYS;
	}

	if (fail_over_mac) {
		bond_opt_initstr(&newval, fail_over_mac);
		valptr = bond_opt_parse(bond_opt_get(BOND_OPT_FAIL_OVER_MAC),
					&newval);
		if (!valptr) {
			pr_err("Error: invalid fail_over_mac \"%s\"\n",
			       fail_over_mac);
			return -EINVAL;
		}
		fail_over_mac_value = valptr->value;
		if (bond_mode != BOND_MODE_ACTIVEBACKUP)
			pr_warn("Warning: fail_over_mac only affects active-backup mode\n");
	} else {
		fail_over_mac_value = BOND_FOM_NONE;
	}

	bond_opt_initstr(&newval, "default");
	valptr = bond_opt_parse(
			bond_opt_get(BOND_OPT_AD_ACTOR_SYS_PRIO),
				     &newval);
	if (!valptr) {
		pr_err("Error: No ad_actor_sys_prio default value");
		return -EINVAL;
	}
	ad_actor_sys_prio = valptr->value;

	valptr = bond_opt_parse(bond_opt_get(BOND_OPT_AD_USER_PORT_KEY),
				&newval);
	if (!valptr) {
		pr_err("Error: No ad_user_port_key default value");
		return -EINVAL;
	}
	ad_user_port_key = valptr->value;

	bond_opt_initstr(&newval, "default");
	valptr = bond_opt_parse(bond_opt_get(BOND_OPT_TLB_DYNAMIC_LB), &newval);
	if (!valptr) {
		pr_err("Error: No tlb_dynamic_lb default value");
		return -EINVAL;
	}
	tlb_dynamic_lb = valptr->value;

	if (lp_interval == 0) {
		pr_warn("Warning: ip_interval must be between 1 and %d, so it was reset to %d\n",
			INT_MAX, BOND_ALB_DEFAULT_LP_INTERVAL);
		lp_interval = BOND_ALB_DEFAULT_LP_INTERVAL;
	}

	/* fill params struct with the proper values */
	params->mode = bond_mode;
	params->xmit_policy = xmit_hashtype;
	params->miimon = miimon;
	params->num_peer_notif = num_peer_notif;
	params->arp_interval = arp_interval;
	params->arp_validate = arp_validate_value;
	params->arp_all_targets = arp_all_targets_value;
	params->updelay = updelay;
	params->downdelay = downdelay;
	params->peer_notif_delay = 0;
	params->use_carrier = use_carrier;
	params->lacp_fast = lacp_fast;
	params->primary[0] = 0;
	params->primary_reselect = primary_reselect_value;
	params->fail_over_mac = fail_over_mac_value;
	params->tx_queues = tx_queues;
	params->all_slaves_active = all_slaves_active;
	params->resend_igmp = resend_igmp;
	params->min_links = min_links;
	params->lp_interval = lp_interval;
	params->packets_per_slave = packets_per_slave;
	params->tlb_dynamic_lb = tlb_dynamic_lb;
	params->ad_actor_sys_prio = ad_actor_sys_prio;
	eth_zero_addr(params->ad_actor_system);
	params->ad_user_port_key = ad_user_port_key;
	if (packets_per_slave > 0) {
		params->reciprocal_packets_per_slave =
			reciprocal_value(packets_per_slave);
	} else {
		/* reciprocal_packets_per_slave is unused if
		 * packets_per_slave is 0 or 1, just initialize it
		 */
		params->reciprocal_packets_per_slave =
			(struct reciprocal_value) { 0 };
	}

	if (primary)
		strscpy_pad(params->primary, primary, sizeof(params->primary));

	memcpy(params->arp_targets, arp_target, sizeof(arp_target));

	return 0;
}
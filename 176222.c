static int ax88179_chk_eee(struct usbnet *dev)
{
	struct ethtool_cmd ecmd = { .cmd = ETHTOOL_GSET };
	struct ax88179_data *priv = (struct ax88179_data *)dev->data;

	mii_ethtool_gset(&dev->mii, &ecmd);

	if (ecmd.duplex & DUPLEX_FULL) {
		int eee_lp, eee_cap, eee_adv;
		u32 lp, cap, adv, supported = 0;

		eee_cap = ax88179_phy_read_mmd_indirect(dev,
							MDIO_PCS_EEE_ABLE,
							MDIO_MMD_PCS);
		if (eee_cap < 0) {
			priv->eee_active = 0;
			return false;
		}

		cap = mmd_eee_cap_to_ethtool_sup_t(eee_cap);
		if (!cap) {
			priv->eee_active = 0;
			return false;
		}

		eee_lp = ax88179_phy_read_mmd_indirect(dev,
						       MDIO_AN_EEE_LPABLE,
						       MDIO_MMD_AN);
		if (eee_lp < 0) {
			priv->eee_active = 0;
			return false;
		}

		eee_adv = ax88179_phy_read_mmd_indirect(dev,
							MDIO_AN_EEE_ADV,
							MDIO_MMD_AN);

		if (eee_adv < 0) {
			priv->eee_active = 0;
			return false;
		}

		adv = mmd_eee_adv_to_ethtool_adv_t(eee_adv);
		lp = mmd_eee_adv_to_ethtool_adv_t(eee_lp);
		supported = (ecmd.speed == SPEED_1000) ?
			     SUPPORTED_1000baseT_Full :
			     SUPPORTED_100baseT_Full;

		if (!(lp & adv & supported)) {
			priv->eee_active = 0;
			return false;
		}

		priv->eee_active = 1;
		return true;
	}

	priv->eee_active = 0;
	return false;
}
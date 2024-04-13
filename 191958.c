static void rtl8723bu_handle_c2h(struct rtl8xxxu_priv *priv,
				 struct sk_buff *skb)
{
	struct rtl8723bu_c2h *c2h = (struct rtl8723bu_c2h *)skb->data;
	struct device *dev = &priv->udev->dev;
	int len;

	len = skb->len - 2;

	dev_dbg(dev, "C2H ID %02x seq %02x, len %02x source %02x\n",
		c2h->id, c2h->seq, len, c2h->bt_info.response_source);

	switch(c2h->id) {
	case C2H_8723B_BT_INFO:
		if (c2h->bt_info.response_source >
		    BT_INFO_SRC_8723B_BT_ACTIVE_SEND)
			dev_dbg(dev, "C2H_BT_INFO WiFi only firmware\n");
		else
			dev_dbg(dev, "C2H_BT_INFO BT/WiFi coexist firmware\n");

		if (c2h->bt_info.bt_has_reset)
			dev_dbg(dev, "BT has been reset\n");
		if (c2h->bt_info.tx_rx_mask)
			dev_dbg(dev, "BT TRx mask\n");

		break;
	case C2H_8723B_BT_MP_INFO:
		dev_dbg(dev, "C2H_MP_INFO ext ID %02x, status %02x\n",
			c2h->bt_mp_info.ext_id, c2h->bt_mp_info.status);
		break;
	case C2H_8723B_RA_REPORT:
		dev_dbg(dev,
			"C2H RA RPT: rate %02x, unk %i, macid %02x, noise %i\n",
			c2h->ra_report.rate, c2h->ra_report.dummy0_0,
			c2h->ra_report.macid, c2h->ra_report.noisy_state);
		break;
	default:
		dev_info(dev, "Unhandled C2H event %02x seq %02x\n",
			 c2h->id, c2h->seq);
		print_hex_dump(KERN_INFO, "C2H content: ", DUMP_PREFIX_NONE,
			       16, 1, c2h->raw.payload, len, false);
		break;
	}
}
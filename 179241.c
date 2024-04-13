static int gen_ndis_set_resp(struct rndis_params *params, u32 OID,
			     u8 *buf, u32 buf_len, rndis_resp_t *r)
{
	rndis_set_cmplt_type *resp;
	int i, retval = -ENOTSUPP;

	if (!r)
		return -ENOMEM;
	resp = (rndis_set_cmplt_type *)r->buf;
	if (!resp)
		return -ENOMEM;

	if (buf_len && rndis_debug > 1) {
		pr_debug("set OID %08x value, len %d:\n", OID, buf_len);
		for (i = 0; i < buf_len; i += 16) {
			pr_debug("%03d: %08x %08x %08x %08x\n", i,
				get_unaligned_le32(&buf[i]),
				get_unaligned_le32(&buf[i + 4]),
				get_unaligned_le32(&buf[i + 8]),
				get_unaligned_le32(&buf[i + 12]));
		}
	}

	switch (OID) {
	case RNDIS_OID_GEN_CURRENT_PACKET_FILTER:

		/* these NDIS_PACKET_TYPE_* bitflags are shared with
		 * cdc_filter; it's not RNDIS-specific
		 * NDIS_PACKET_TYPE_x == USB_CDC_PACKET_TYPE_x for x in:
		 *	PROMISCUOUS, DIRECTED,
		 *	MULTICAST, ALL_MULTICAST, BROADCAST
		 */
		*params->filter = (u16)get_unaligned_le32(buf);
		pr_debug("%s: RNDIS_OID_GEN_CURRENT_PACKET_FILTER %08x\n",
			__func__, *params->filter);

		/* this call has a significant side effect:  it's
		 * what makes the packet flow start and stop, like
		 * activating the CDC Ethernet altsetting.
		 */
		retval = 0;
		if (*params->filter) {
			params->state = RNDIS_DATA_INITIALIZED;
			netif_carrier_on(params->dev);
			if (netif_running(params->dev))
				netif_wake_queue(params->dev);
		} else {
			params->state = RNDIS_INITIALIZED;
			netif_carrier_off(params->dev);
			netif_stop_queue(params->dev);
		}
		break;

	case RNDIS_OID_802_3_MULTICAST_LIST:
		/* I think we can ignore this */
		pr_debug("%s: RNDIS_OID_802_3_MULTICAST_LIST\n", __func__);
		retval = 0;
		break;

	default:
		pr_warn("%s: set unknown OID 0x%08X, size %d\n",
			__func__, OID, buf_len);
	}

	return retval;
}
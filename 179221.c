static int gen_ndis_query_resp(struct rndis_params *params, u32 OID, u8 *buf,
			       unsigned buf_len, rndis_resp_t *r)
{
	int retval = -ENOTSUPP;
	u32 length = 4;	/* usually */
	__le32 *outbuf;
	int i, count;
	rndis_query_cmplt_type *resp;
	struct net_device *net;
	struct rtnl_link_stats64 temp;
	const struct rtnl_link_stats64 *stats;

	if (!r) return -ENOMEM;
	resp = (rndis_query_cmplt_type *)r->buf;

	if (!resp) return -ENOMEM;

	if (buf_len && rndis_debug > 1) {
		pr_debug("query OID %08x value, len %d:\n", OID, buf_len);
		for (i = 0; i < buf_len; i += 16) {
			pr_debug("%03d: %08x %08x %08x %08x\n", i,
				get_unaligned_le32(&buf[i]),
				get_unaligned_le32(&buf[i + 4]),
				get_unaligned_le32(&buf[i + 8]),
				get_unaligned_le32(&buf[i + 12]));
		}
	}

	/* response goes here, right after the header */
	outbuf = (__le32 *)&resp[1];
	resp->InformationBufferOffset = cpu_to_le32(16);

	net = params->dev;
	stats = dev_get_stats(net, &temp);

	switch (OID) {

	/* general oids (table 4-1) */

	/* mandatory */
	case RNDIS_OID_GEN_SUPPORTED_LIST:
		pr_debug("%s: RNDIS_OID_GEN_SUPPORTED_LIST\n", __func__);
		length = sizeof(oid_supported_list);
		count  = length / sizeof(u32);
		for (i = 0; i < count; i++)
			outbuf[i] = cpu_to_le32(oid_supported_list[i]);
		retval = 0;
		break;

	/* mandatory */
	case RNDIS_OID_GEN_HARDWARE_STATUS:
		pr_debug("%s: RNDIS_OID_GEN_HARDWARE_STATUS\n", __func__);
		/* Bogus question!
		 * Hardware must be ready to receive high level protocols.
		 * BTW:
		 * reddite ergo quae sunt Caesaris Caesari
		 * et quae sunt Dei Deo!
		 */
		*outbuf = cpu_to_le32(0);
		retval = 0;
		break;

	/* mandatory */
	case RNDIS_OID_GEN_MEDIA_SUPPORTED:
		pr_debug("%s: RNDIS_OID_GEN_MEDIA_SUPPORTED\n", __func__);
		*outbuf = cpu_to_le32(params->medium);
		retval = 0;
		break;

	/* mandatory */
	case RNDIS_OID_GEN_MEDIA_IN_USE:
		pr_debug("%s: RNDIS_OID_GEN_MEDIA_IN_USE\n", __func__);
		/* one medium, one transport... (maybe you do it better) */
		*outbuf = cpu_to_le32(params->medium);
		retval = 0;
		break;

	/* mandatory */
	case RNDIS_OID_GEN_MAXIMUM_FRAME_SIZE:
		pr_debug("%s: RNDIS_OID_GEN_MAXIMUM_FRAME_SIZE\n", __func__);
		if (params->dev) {
			*outbuf = cpu_to_le32(params->dev->mtu);
			retval = 0;
		}
		break;

	/* mandatory */
	case RNDIS_OID_GEN_LINK_SPEED:
		if (rndis_debug > 1)
			pr_debug("%s: RNDIS_OID_GEN_LINK_SPEED\n", __func__);
		if (params->media_state == RNDIS_MEDIA_STATE_DISCONNECTED)
			*outbuf = cpu_to_le32(0);
		else
			*outbuf = cpu_to_le32(params->speed);
		retval = 0;
		break;

	/* mandatory */
	case RNDIS_OID_GEN_TRANSMIT_BLOCK_SIZE:
		pr_debug("%s: RNDIS_OID_GEN_TRANSMIT_BLOCK_SIZE\n", __func__);
		if (params->dev) {
			*outbuf = cpu_to_le32(params->dev->mtu);
			retval = 0;
		}
		break;

	/* mandatory */
	case RNDIS_OID_GEN_RECEIVE_BLOCK_SIZE:
		pr_debug("%s: RNDIS_OID_GEN_RECEIVE_BLOCK_SIZE\n", __func__);
		if (params->dev) {
			*outbuf = cpu_to_le32(params->dev->mtu);
			retval = 0;
		}
		break;

	/* mandatory */
	case RNDIS_OID_GEN_VENDOR_ID:
		pr_debug("%s: RNDIS_OID_GEN_VENDOR_ID\n", __func__);
		*outbuf = cpu_to_le32(params->vendorID);
		retval = 0;
		break;

	/* mandatory */
	case RNDIS_OID_GEN_VENDOR_DESCRIPTION:
		pr_debug("%s: RNDIS_OID_GEN_VENDOR_DESCRIPTION\n", __func__);
		if (params->vendorDescr) {
			length = strlen(params->vendorDescr);
			memcpy(outbuf, params->vendorDescr, length);
		} else {
			outbuf[0] = 0;
		}
		retval = 0;
		break;

	case RNDIS_OID_GEN_VENDOR_DRIVER_VERSION:
		pr_debug("%s: RNDIS_OID_GEN_VENDOR_DRIVER_VERSION\n", __func__);
		/* Created as LE */
		*outbuf = rndis_driver_version;
		retval = 0;
		break;

	/* mandatory */
	case RNDIS_OID_GEN_CURRENT_PACKET_FILTER:
		pr_debug("%s: RNDIS_OID_GEN_CURRENT_PACKET_FILTER\n", __func__);
		*outbuf = cpu_to_le32(*params->filter);
		retval = 0;
		break;

	/* mandatory */
	case RNDIS_OID_GEN_MAXIMUM_TOTAL_SIZE:
		pr_debug("%s: RNDIS_OID_GEN_MAXIMUM_TOTAL_SIZE\n", __func__);
		*outbuf = cpu_to_le32(RNDIS_MAX_TOTAL_SIZE);
		retval = 0;
		break;

	/* mandatory */
	case RNDIS_OID_GEN_MEDIA_CONNECT_STATUS:
		if (rndis_debug > 1)
			pr_debug("%s: RNDIS_OID_GEN_MEDIA_CONNECT_STATUS\n", __func__);
		*outbuf = cpu_to_le32(params->media_state);
		retval = 0;
		break;

	case RNDIS_OID_GEN_PHYSICAL_MEDIUM:
		pr_debug("%s: RNDIS_OID_GEN_PHYSICAL_MEDIUM\n", __func__);
		*outbuf = cpu_to_le32(0);
		retval = 0;
		break;

	/* The RNDIS specification is incomplete/wrong.   Some versions
	 * of MS-Windows expect OIDs that aren't specified there.  Other
	 * versions emit undefined RNDIS messages. DOCUMENT ALL THESE!
	 */
	case RNDIS_OID_GEN_MAC_OPTIONS:		/* from WinME */
		pr_debug("%s: RNDIS_OID_GEN_MAC_OPTIONS\n", __func__);
		*outbuf = cpu_to_le32(
			  RNDIS_MAC_OPTION_RECEIVE_SERIALIZED
			| RNDIS_MAC_OPTION_FULL_DUPLEX);
		retval = 0;
		break;

	/* statistics OIDs (table 4-2) */

	/* mandatory */
	case RNDIS_OID_GEN_XMIT_OK:
		if (rndis_debug > 1)
			pr_debug("%s: RNDIS_OID_GEN_XMIT_OK\n", __func__);
		if (stats) {
			*outbuf = cpu_to_le32(stats->tx_packets
				- stats->tx_errors - stats->tx_dropped);
			retval = 0;
		}
		break;

	/* mandatory */
	case RNDIS_OID_GEN_RCV_OK:
		if (rndis_debug > 1)
			pr_debug("%s: RNDIS_OID_GEN_RCV_OK\n", __func__);
		if (stats) {
			*outbuf = cpu_to_le32(stats->rx_packets
				- stats->rx_errors - stats->rx_dropped);
			retval = 0;
		}
		break;

	/* mandatory */
	case RNDIS_OID_GEN_XMIT_ERROR:
		if (rndis_debug > 1)
			pr_debug("%s: RNDIS_OID_GEN_XMIT_ERROR\n", __func__);
		if (stats) {
			*outbuf = cpu_to_le32(stats->tx_errors);
			retval = 0;
		}
		break;

	/* mandatory */
	case RNDIS_OID_GEN_RCV_ERROR:
		if (rndis_debug > 1)
			pr_debug("%s: RNDIS_OID_GEN_RCV_ERROR\n", __func__);
		if (stats) {
			*outbuf = cpu_to_le32(stats->rx_errors);
			retval = 0;
		}
		break;

	/* mandatory */
	case RNDIS_OID_GEN_RCV_NO_BUFFER:
		pr_debug("%s: RNDIS_OID_GEN_RCV_NO_BUFFER\n", __func__);
		if (stats) {
			*outbuf = cpu_to_le32(stats->rx_dropped);
			retval = 0;
		}
		break;

	/* ieee802.3 OIDs (table 4-3) */

	/* mandatory */
	case RNDIS_OID_802_3_PERMANENT_ADDRESS:
		pr_debug("%s: RNDIS_OID_802_3_PERMANENT_ADDRESS\n", __func__);
		if (params->dev) {
			length = ETH_ALEN;
			memcpy(outbuf, params->host_mac, length);
			retval = 0;
		}
		break;

	/* mandatory */
	case RNDIS_OID_802_3_CURRENT_ADDRESS:
		pr_debug("%s: RNDIS_OID_802_3_CURRENT_ADDRESS\n", __func__);
		if (params->dev) {
			length = ETH_ALEN;
			memcpy(outbuf, params->host_mac, length);
			retval = 0;
		}
		break;

	/* mandatory */
	case RNDIS_OID_802_3_MULTICAST_LIST:
		pr_debug("%s: RNDIS_OID_802_3_MULTICAST_LIST\n", __func__);
		/* Multicast base address only */
		*outbuf = cpu_to_le32(0xE0000000);
		retval = 0;
		break;

	/* mandatory */
	case RNDIS_OID_802_3_MAXIMUM_LIST_SIZE:
		pr_debug("%s: RNDIS_OID_802_3_MAXIMUM_LIST_SIZE\n", __func__);
		/* Multicast base address only */
		*outbuf = cpu_to_le32(1);
		retval = 0;
		break;

	case RNDIS_OID_802_3_MAC_OPTIONS:
		pr_debug("%s: RNDIS_OID_802_3_MAC_OPTIONS\n", __func__);
		*outbuf = cpu_to_le32(0);
		retval = 0;
		break;

	/* ieee802.3 statistics OIDs (table 4-4) */

	/* mandatory */
	case RNDIS_OID_802_3_RCV_ERROR_ALIGNMENT:
		pr_debug("%s: RNDIS_OID_802_3_RCV_ERROR_ALIGNMENT\n", __func__);
		if (stats) {
			*outbuf = cpu_to_le32(stats->rx_frame_errors);
			retval = 0;
		}
		break;

	/* mandatory */
	case RNDIS_OID_802_3_XMIT_ONE_COLLISION:
		pr_debug("%s: RNDIS_OID_802_3_XMIT_ONE_COLLISION\n", __func__);
		*outbuf = cpu_to_le32(0);
		retval = 0;
		break;

	/* mandatory */
	case RNDIS_OID_802_3_XMIT_MORE_COLLISIONS:
		pr_debug("%s: RNDIS_OID_802_3_XMIT_MORE_COLLISIONS\n", __func__);
		*outbuf = cpu_to_le32(0);
		retval = 0;
		break;

	default:
		pr_warn("%s: query unknown OID 0x%08X\n", __func__, OID);
	}
	if (retval < 0)
		length = 0;

	resp->InformationBufferLength = cpu_to_le32(length);
	r->length = length + sizeof(*resp);
	resp->MessageLength = cpu_to_le32(r->length);
	return retval;
}
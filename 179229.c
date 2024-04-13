int rndis_msg_parser(struct rndis_params *params, u8 *buf)
{
	u32 MsgType, MsgLength;
	__le32 *tmp;

	if (!buf)
		return -ENOMEM;

	tmp = (__le32 *)buf;
	MsgType   = get_unaligned_le32(tmp++);
	MsgLength = get_unaligned_le32(tmp++);

	if (!params)
		return -ENOTSUPP;

	/* NOTE: RNDIS is *EXTREMELY* chatty ... Windows constantly polls for
	 * rx/tx statistics and link status, in addition to KEEPALIVE traffic
	 * and normal HC level polling to see if there's any IN traffic.
	 */

	/* For USB: responses may take up to 10 seconds */
	switch (MsgType) {
	case RNDIS_MSG_INIT:
		pr_debug("%s: RNDIS_MSG_INIT\n",
			__func__);
		params->state = RNDIS_INITIALIZED;
		return rndis_init_response(params, (rndis_init_msg_type *)buf);

	case RNDIS_MSG_HALT:
		pr_debug("%s: RNDIS_MSG_HALT\n",
			__func__);
		params->state = RNDIS_UNINITIALIZED;
		if (params->dev) {
			netif_carrier_off(params->dev);
			netif_stop_queue(params->dev);
		}
		return 0;

	case RNDIS_MSG_QUERY:
		return rndis_query_response(params,
					(rndis_query_msg_type *)buf);

	case RNDIS_MSG_SET:
		return rndis_set_response(params, (rndis_set_msg_type *)buf);

	case RNDIS_MSG_RESET:
		pr_debug("%s: RNDIS_MSG_RESET\n",
			__func__);
		return rndis_reset_response(params,
					(rndis_reset_msg_type *)buf);

	case RNDIS_MSG_KEEPALIVE:
		/* For USB: host does this every 5 seconds */
		if (rndis_debug > 1)
			pr_debug("%s: RNDIS_MSG_KEEPALIVE\n",
				__func__);
		return rndis_keepalive_response(params,
						 (rndis_keepalive_msg_type *)
						 buf);

	default:
		/* At least Windows XP emits some undefined RNDIS messages.
		 * In one case those messages seemed to relate to the host
		 * suspending itself.
		 */
		pr_warn("%s: unknown RNDIS message 0x%08X len %d\n",
			__func__, MsgType, MsgLength);
		/* Garbled message can be huge, so limit what we display */
		if (MsgLength > 16)
			MsgLength = 16;
		print_hex_dump_bytes(__func__, DUMP_PREFIX_OFFSET,
				     buf, MsgLength);
		break;
	}

	return -ENOTSUPP;
}
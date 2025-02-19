int i2400m_op_rfkill_sw_toggle(struct wimax_dev *wimax_dev,
			       enum wimax_rf_state state)
{
	int result;
	struct i2400m *i2400m = wimax_dev_to_i2400m(wimax_dev);
	struct device *dev = i2400m_dev(i2400m);
	struct sk_buff *ack_skb;
	struct {
		struct i2400m_l3l4_hdr hdr;
		struct i2400m_tlv_rf_operation sw_rf;
	} __packed *cmd;
	char strerr[32];

	d_fnstart(4, dev, "(wimax_dev %p state %d)\n", wimax_dev, state);

	result = -ENOMEM;
	cmd = kzalloc(sizeof(*cmd), GFP_KERNEL);
	if (cmd == NULL)
		goto error_alloc;
	cmd->hdr.type = cpu_to_le16(I2400M_MT_CMD_RF_CONTROL);
	cmd->hdr.length = sizeof(cmd->sw_rf);
	cmd->hdr.version = cpu_to_le16(I2400M_L3L4_VERSION);
	cmd->sw_rf.hdr.type = cpu_to_le16(I2400M_TLV_RF_OPERATION);
	cmd->sw_rf.hdr.length = cpu_to_le16(sizeof(cmd->sw_rf.status));
	switch (state) {
	case WIMAX_RF_OFF:	/* RFKILL ON, radio OFF */
		cmd->sw_rf.status = cpu_to_le32(2);
		break;
	case WIMAX_RF_ON:	/* RFKILL OFF, radio ON */
		cmd->sw_rf.status = cpu_to_le32(1);
		break;
	default:
		BUG();
	}

	ack_skb = i2400m_msg_to_dev(i2400m, cmd, sizeof(*cmd));
	result = PTR_ERR(ack_skb);
	if (IS_ERR(ack_skb)) {
		dev_err(dev, "Failed to issue 'RF Control' command: %d\n",
			result);
		goto error_msg_to_dev;
	}
	result = i2400m_msg_check_status(wimax_msg_data(ack_skb),
					 strerr, sizeof(strerr));
	if (result < 0) {
		dev_err(dev, "'RF Control' (0x%04x) command failed: %d - %s\n",
			I2400M_MT_CMD_RF_CONTROL, result, strerr);
		goto error_cmd;
	}

	/* Now we wait for the state to change to RADIO_OFF or RADIO_ON */
	result = wait_event_timeout(
		i2400m->state_wq, i2400m_radio_is(i2400m, state),
		5 * HZ);
	if (result == 0)
		result = -ETIMEDOUT;
	if (result < 0)
		dev_err(dev, "Error waiting for device to toggle RF state: "
			"%d\n", result);
	result = 0;
error_cmd:
	kfree_skb(ack_skb);
error_msg_to_dev:
error_alloc:
	d_fnend(4, dev, "(wimax_dev %p state %d) = %d\n",
		wimax_dev, state, result);
	kfree(cmd);
	return result;
}
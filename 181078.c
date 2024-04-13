int ath6kl_wmi_addkey_cmd(struct wmi *wmi, u8 if_idx, u8 key_index,
			  enum ath6kl_crypto_type key_type,
			  u8 key_usage, u8 key_len,
			  u8 *key_rsc, unsigned int key_rsc_len,
			  u8 *key_material,
			  u8 key_op_ctrl, u8 *mac_addr,
			  enum wmi_sync_flag sync_flag)
{
	struct sk_buff *skb;
	struct wmi_add_cipher_key_cmd *cmd;
	int ret;

	ath6kl_dbg(ATH6KL_DBG_WMI,
		   "addkey cmd: key_index=%u key_type=%d key_usage=%d key_len=%d key_op_ctrl=%d\n",
		   key_index, key_type, key_usage, key_len, key_op_ctrl);

	if ((key_index > WMI_MAX_KEY_INDEX) || (key_len > WMI_MAX_KEY_LEN) ||
	    (key_material == NULL) || key_rsc_len > 8)
		return -EINVAL;

	if ((WEP_CRYPT != key_type) && (NULL == key_rsc))
		return -EINVAL;

	skb = ath6kl_wmi_get_new_buf(sizeof(*cmd));
	if (!skb)
		return -ENOMEM;

	cmd = (struct wmi_add_cipher_key_cmd *) skb->data;
	cmd->key_index = key_index;
	cmd->key_type = key_type;
	cmd->key_usage = key_usage;
	cmd->key_len = key_len;
	memcpy(cmd->key, key_material, key_len);

	if (key_rsc != NULL)
		memcpy(cmd->key_rsc, key_rsc, key_rsc_len);

	cmd->key_op_ctrl = key_op_ctrl;

	if (mac_addr)
		memcpy(cmd->key_mac_addr, mac_addr, ETH_ALEN);

	ret = ath6kl_wmi_cmd_send(wmi, if_idx, skb, WMI_ADD_CIPHER_KEY_CMDID,
				  sync_flag);

	return ret;
}
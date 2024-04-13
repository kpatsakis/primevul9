int wmi_add_cipher_key(struct wil6210_priv *wil, u8 key_index,
		       const void *mac_addr, int key_len, const void *key,
		       int key_usage)
{
	struct wmi_add_cipher_key_cmd cmd = {
		.key_index = key_index,
		.key_usage = key_usage,
		.key_len = key_len,
	};

	if (!key || (key_len > sizeof(cmd.key)))
		return -EINVAL;

	memcpy(cmd.key, key, key_len);
	if (mac_addr)
		memcpy(cmd.mac, mac_addr, WMI_MAC_LEN);

	return wmi_send(wil, WMI_ADD_CIPHER_KEY_CMDID, &cmd, sizeof(cmd));
}
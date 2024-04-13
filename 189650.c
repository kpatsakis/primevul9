int wmi_del_cipher_key(struct wil6210_priv *wil, u8 key_index,
		       const void *mac_addr, int key_usage)
{
	struct wmi_delete_cipher_key_cmd cmd = {
		.key_index = key_index,
	};

	if (mac_addr)
		memcpy(cmd.mac, mac_addr, WMI_MAC_LEN);

	return wmi_send(wil, WMI_DELETE_CIPHER_KEY_CMDID, &cmd, sizeof(cmd));
}
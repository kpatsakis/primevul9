int wmi_set_mac_address(struct wil6210_priv *wil, void *addr)
{
	struct wmi_set_mac_address_cmd cmd;

	ether_addr_copy(cmd.mac, addr);

	wil_dbg_wmi(wil, "Set MAC %pM\n", addr);

	return wmi_send(wil, WMI_SET_MAC_ADDRESS_CMDID, &cmd, sizeof(cmd));
}
static void wmi_evt_ignore(struct wil6210_priv *wil, int id, void *d, int len)
{
	wil_dbg_wmi(wil, "Ignore event 0x%04x len %d\n", id, len);
}
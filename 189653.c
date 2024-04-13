static bool wmi_evt_call_handler(struct wil6210_priv *wil, int id,
				 void *d, int len)
{
	uint i;

	for (i = 0; i < ARRAY_SIZE(wmi_evt_handlers); i++) {
		if (wmi_evt_handlers[i].eventid == id) {
			wmi_evt_handlers[i].handler(wil, id, d, len);
			return true;
		}
	}

	return false;
}
int wmi_send(struct wil6210_priv *wil, u16 cmdid, void *buf, u16 len)
{
	int rc;

	mutex_lock(&wil->wmi_mutex);
	rc = __wmi_send(wil, cmdid, buf, len);
	mutex_unlock(&wil->wmi_mutex);

	return rc;
}
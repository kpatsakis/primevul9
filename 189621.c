void __iomem *wmi_addr(struct wil6210_priv *wil, u32 ptr)
{
	u32 off;

	if (ptr % 4)
		return NULL;

	if (ptr < WIL6210_FW_HOST_OFF)
		return NULL;

	off = HOSTADDR(ptr);
	if (off > wil->bar_size - 4)
		return NULL;

	return wil->csr + off;
}
void __iomem *wmi_buffer(struct wil6210_priv *wil, __le32 ptr_)
{
	return wmi_buffer_block(wil, ptr_, 0);
}
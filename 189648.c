void __iomem *wmi_buffer_block(struct wil6210_priv *wil, __le32 ptr_, u32 size)
{
	u32 off;
	u32 ptr = le32_to_cpu(ptr_);

	if (ptr % 4)
		return NULL;

	ptr = wmi_addr_remap(ptr);
	if (ptr < WIL6210_FW_HOST_OFF)
		return NULL;

	off = HOSTADDR(ptr);
	if (off > wil->bar_size - 4)
		return NULL;
	if (size && ((off + size > wil->bar_size) || (off + size < off)))
		return NULL;

	return wil->csr + off;
}
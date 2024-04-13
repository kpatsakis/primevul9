int wmi_read_hdr(struct wil6210_priv *wil, __le32 ptr,
		 struct wil6210_mbox_hdr *hdr)
{
	void __iomem *src = wmi_buffer(wil, ptr);

	if (!src)
		return -EINVAL;

	wil_memcpy_fromio_32(hdr, src, sizeof(*hdr));

	return 0;
}
static int intel_vgpu_bar_rw(struct intel_vgpu *vgpu, int bar, uint64_t off,
			     void *buf, unsigned int count, bool is_write)
{
	uint64_t bar_start = intel_vgpu_get_bar_addr(vgpu, bar);
	int ret;

	if (is_write)
		ret = intel_gvt_ops->emulate_mmio_write(vgpu,
					bar_start + off, buf, count);
	else
		ret = intel_gvt_ops->emulate_mmio_read(vgpu,
					bar_start + off, buf, count);
	return ret;
}
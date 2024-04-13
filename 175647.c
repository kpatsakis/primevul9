static int intel_vgpu_aperture_rw(struct intel_vgpu *vgpu, uint64_t off,
		void *buf, unsigned long count, bool is_write)
{
	void *aperture_va;

	if (!intel_vgpu_in_aperture(vgpu, off) ||
	    !intel_vgpu_in_aperture(vgpu, off + count)) {
		gvt_vgpu_err("Invalid aperture offset %llu\n", off);
		return -EINVAL;
	}

	aperture_va = io_mapping_map_wc(&vgpu->gvt->dev_priv->ggtt.iomap,
					ALIGN_DOWN(off, PAGE_SIZE),
					count + offset_in_page(off));
	if (!aperture_va)
		return -EIO;

	if (is_write)
		memcpy(aperture_va + offset_in_page(off), buf, count);
	else
		memcpy(buf, aperture_va + offset_in_page(off), count);

	io_mapping_unmap(aperture_va);

	return 0;
}
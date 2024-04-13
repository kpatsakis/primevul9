static inline bool intel_vgpu_in_aperture(struct intel_vgpu *vgpu, uint64_t off)
{
	return off >= vgpu_aperture_offset(vgpu) &&
	       off < vgpu_aperture_offset(vgpu) + vgpu_aperture_sz(vgpu);
}
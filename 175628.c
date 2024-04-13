static void intel_vgpu_release_work(struct work_struct *work)
{
	struct intel_vgpu *vgpu = container_of(work, struct intel_vgpu,
					vdev.release_work);

	__intel_vgpu_release(vgpu);
}
static void intel_vgpu_release_msi_eventfd_ctx(struct intel_vgpu *vgpu)
{
	struct eventfd_ctx *trigger;

	trigger = vgpu->vdev.msi_trigger;
	if (trigger) {
		eventfd_ctx_put(trigger);
		vgpu->vdev.msi_trigger = NULL;
	}
}
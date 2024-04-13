static int intel_vgpu_set_msi_trigger(struct intel_vgpu *vgpu,
		unsigned int index, unsigned int start, unsigned int count,
		uint32_t flags, void *data)
{
	struct eventfd_ctx *trigger;

	if (flags & VFIO_IRQ_SET_DATA_EVENTFD) {
		int fd = *(int *)data;

		trigger = eventfd_ctx_fdget(fd);
		if (IS_ERR(trigger)) {
			gvt_vgpu_err("eventfd_ctx_fdget failed\n");
			return PTR_ERR(trigger);
		}
		vgpu->vdev.msi_trigger = trigger;
	} else if ((flags & VFIO_IRQ_SET_DATA_NONE) && !count)
		intel_vgpu_release_msi_eventfd_ctx(vgpu);

	return 0;
}
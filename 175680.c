static int intel_vgpu_group_notifier(struct notifier_block *nb,
				     unsigned long action, void *data)
{
	struct intel_vgpu *vgpu = container_of(nb,
					struct intel_vgpu,
					vdev.group_notifier);

	/* the only action we care about */
	if (action == VFIO_GROUP_NOTIFY_SET_KVM) {
		vgpu->vdev.kvm = data;

		if (!data)
			schedule_work(&vgpu->vdev.release_work);
	}

	return NOTIFY_OK;
}
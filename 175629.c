static bool __kvmgt_vgpu_exist(struct intel_vgpu *vgpu, struct kvm *kvm)
{
	struct intel_vgpu *itr;
	struct kvmgt_guest_info *info;
	int id;
	bool ret = false;

	mutex_lock(&vgpu->gvt->lock);
	for_each_active_vgpu(vgpu->gvt, itr, id) {
		if (!handle_valid(itr->handle))
			continue;

		info = (struct kvmgt_guest_info *)itr->handle;
		if (kvm && kvm == info->kvm) {
			ret = true;
			goto out;
		}
	}
out:
	mutex_unlock(&vgpu->gvt->lock);
	return ret;
}
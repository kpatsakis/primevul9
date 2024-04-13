static void kvmgt_page_track_write(struct kvm_vcpu *vcpu, gpa_t gpa,
		const u8 *val, int len,
		struct kvm_page_track_notifier_node *node)
{
	struct kvmgt_guest_info *info = container_of(node,
					struct kvmgt_guest_info, track_node);

	if (kvmgt_gfn_is_write_protected(info, gpa_to_gfn(gpa)))
		intel_gvt_ops->write_protect_handler(info->vgpu, gpa,
						     (void *)val, len);
}
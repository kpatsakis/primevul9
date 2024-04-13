static int kvmgt_host_init(struct device *dev, void *gvt, const void *ops)
{
	struct attribute **kvm_type_attrs;
	struct attribute_group **kvm_vgpu_type_groups;

	intel_gvt_ops = ops;
	if (!intel_gvt_ops->get_gvt_attrs(&kvm_type_attrs,
			&kvm_vgpu_type_groups))
		return -EFAULT;
	intel_vgpu_ops.supported_type_groups = kvm_vgpu_type_groups;

	return mdev_register_device(dev, &intel_vgpu_ops);
}
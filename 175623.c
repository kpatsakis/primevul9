static int intel_vgpu_create(struct kobject *kobj, struct mdev_device *mdev)
{
	struct intel_vgpu *vgpu = NULL;
	struct intel_vgpu_type *type;
	struct device *pdev;
	void *gvt;
	int ret;

	pdev = mdev_parent_dev(mdev);
	gvt = kdev_to_i915(pdev)->gvt;

	type = intel_gvt_ops->gvt_find_vgpu_type(gvt, kobject_name(kobj));
	if (!type) {
		gvt_vgpu_err("failed to find type %s to create\n",
						kobject_name(kobj));
		ret = -EINVAL;
		goto out;
	}

	vgpu = intel_gvt_ops->vgpu_create(gvt, type);
	if (IS_ERR_OR_NULL(vgpu)) {
		ret = vgpu == NULL ? -EFAULT : PTR_ERR(vgpu);
		gvt_err("failed to create intel vgpu: %d\n", ret);
		goto out;
	}

	INIT_WORK(&vgpu->vdev.release_work, intel_vgpu_release_work);

	vgpu->vdev.mdev = mdev;
	mdev_set_drvdata(mdev, vgpu);

	gvt_dbg_core("intel_vgpu_create succeeded for mdev: %s\n",
		     dev_name(mdev_dev(mdev)));
	ret = 0;

out:
	return ret;
}
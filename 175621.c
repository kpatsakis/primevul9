hw_id_show(struct device *dev, struct device_attribute *attr,
	   char *buf)
{
	struct mdev_device *mdev = mdev_from_dev(dev);

	if (mdev) {
		struct intel_vgpu *vgpu = (struct intel_vgpu *)
			mdev_get_drvdata(mdev);
		return sprintf(buf, "%u\n",
			       vgpu->submission.shadow_ctx->hw_id);
	}
	return sprintf(buf, "\n");
}
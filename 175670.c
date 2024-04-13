static void intel_vgpu_release(struct mdev_device *mdev)
{
	struct intel_vgpu *vgpu = mdev_get_drvdata(mdev);

	__intel_vgpu_release(vgpu);
}
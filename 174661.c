void vcs_remove_sysfs(int index)
{
	device_destroy(vc_class, MKDEV(VCS_MAJOR, index + 1));
	device_destroy(vc_class, MKDEV(VCS_MAJOR, index + 65));
	device_destroy(vc_class, MKDEV(VCS_MAJOR, index + 129));
}
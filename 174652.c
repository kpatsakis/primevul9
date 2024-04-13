void vcs_make_sysfs(int index)
{
	device_create(vc_class, NULL, MKDEV(VCS_MAJOR, index + 1), NULL,
		      "vcs%u", index + 1);
	device_create(vc_class, NULL, MKDEV(VCS_MAJOR, index + 65), NULL,
		      "vcsu%u", index + 1);
	device_create(vc_class, NULL, MKDEV(VCS_MAJOR, index + 129), NULL,
		      "vcsa%u", index + 1);
}
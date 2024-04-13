void dlpar_sysfs_exit(void)
{
	sysfs_remove_group(dlpar_kobj, &dlpar_attr_group);
	kobject_put(dlpar_kobj);
}
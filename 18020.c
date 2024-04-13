int dlpar_sysfs_init(void)
{
	int error;

	dlpar_kobj = kobject_create_and_add(DLPAR_KOBJ_NAME,
					    &pci_slots_kset->kobj);
	if (!dlpar_kobj)
		return -EINVAL;

	error = sysfs_create_group(dlpar_kobj, &dlpar_attr_group);
	if (error)
		kobject_put(dlpar_kobj);
	return error;
}
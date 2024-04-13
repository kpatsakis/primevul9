static int __init gadget_cfs_init(void)
{
	int ret;

	config_group_init(&gadget_subsys.su_group);

	ret = configfs_register_subsystem(&gadget_subsys);
	return ret;
}
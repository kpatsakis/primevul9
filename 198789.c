static void __exit gadget_cfs_exit(void)
{
	configfs_unregister_subsystem(&gadget_subsys);
}
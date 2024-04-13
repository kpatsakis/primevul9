static void ext4_exit_feat_adverts(void)
{
	kobject_put(&ext4_feat->f_kobj);
	wait_for_completion(&ext4_feat->f_kobj_unregister);
	kfree(ext4_feat);
}
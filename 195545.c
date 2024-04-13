static void ext4_feat_release(struct kobject *kobj)
{
	complete(&ext4_feat->f_kobj_unregister);
}
static ssize_t ext4_attr_show(struct kobject *kobj,
			      struct attribute *attr, char *buf)
{
	struct ext4_sb_info *sbi = container_of(kobj, struct ext4_sb_info,
						s_kobj);
	struct ext4_attr *a = container_of(attr, struct ext4_attr, attr);

	return a->show ? a->show(a, sbi, buf) : 0;
}
static ssize_t ext4_attr_store(struct kobject *kobj,
			       struct attribute *attr,
			       const char *buf, size_t len)
{
	struct ext4_sb_info *sbi = container_of(kobj, struct ext4_sb_info,
						s_kobj);
	struct ext4_attr *a = container_of(attr, struct ext4_attr, attr);

	return a->store ? a->store(a, sbi, buf, len) : 0;
}
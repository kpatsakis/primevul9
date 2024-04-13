static void kvmgt_protect_table_init(struct kvmgt_guest_info *info)
{
	hash_init(info->ptable);
}
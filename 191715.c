static inline u32 netlink_group_mask(u32 group)
{
	return group ? 1 << (group - 1) : 0;
}
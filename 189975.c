static bool need_remote_flush(u64 old, u64 new)
{
	if (!is_shadow_present_pte(old))
		return false;
	if (!is_shadow_present_pte(new))
		return true;
	if ((old ^ new) & PT64_BASE_ADDR_MASK)
		return true;
	old ^= shadow_nx_mask;
	new ^= shadow_nx_mask;
	return (old & ~new & PT64_PERM_MASK) != 0;
}
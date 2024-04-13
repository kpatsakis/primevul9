bool pm_suspended_storage(void)
{
	if ((gfp_allowed_mask & (__GFP_IO | __GFP_FS)) == (__GFP_IO | __GFP_FS))
		return false;
	return true;
}
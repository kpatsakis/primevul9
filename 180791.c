static bool __need_fs_reclaim(gfp_t gfp_mask)
{
	gfp_mask = current_gfp_context(gfp_mask);

	/* no reclaim without waiting on it */
	if (!(gfp_mask & __GFP_DIRECT_RECLAIM))
		return false;

	/* this guy won't enter reclaim */
	if ((current->flags & PF_MEMALLOC) && !(gfp_mask & __GFP_NOMEMALLOC))
		return false;

	/* We're only interested __GFP_FS allocations for now */
	if (!(gfp_mask & __GFP_FS))
		return false;

	if (gfp_mask & __GFP_NOLOCKDEP)
		return false;

	return true;
}
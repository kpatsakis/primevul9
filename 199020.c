static void shm_open(struct vm_area_struct *vma)
{
	int err = __shm_open(vma);
	/*
	 * We raced in the idr lookup or with shm_destroy().
	 * Either way, the ID is busted.
	 */
	WARN_ON_ONCE(err);
}
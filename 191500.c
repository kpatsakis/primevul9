static void dec_lock_count (void)
{
	if (lock_count > 0) {
		lock_count--;
		if (lock_count == 0) {
			/* Tell nscd when lock count goes to zero,
			   if any of the files were changed.  */
			if (nscd_need_reload) {
				nscd_flush_cache ("passwd");
				nscd_flush_cache ("group");
				nscd_need_reload = false;
			}
#ifdef HAVE_LCKPWDF
			ulckpwdf ();
#endif				/* HAVE_LCKPWDF */
		}
	}
}
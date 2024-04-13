static void system_init(void)
{
	voice_compress();
	voice_dup();
	sort_all();			/* define the time / vertical sequences */
//	if (!tsfirst)
//		return;
//	parsys->nstaff = nstaff;	/* save the number of staves */
}
int safe_free (void **p)	/* __SAFE_FREE_CHECKED__ */
{
  free(*p);		/* __MEM_CHECKED__ */
  *p = 0;
}
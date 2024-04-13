get_page_size(void)
{
#ifdef SC_PAGE_SIZE
	return sysconf(SC_PAGE_SIZE);
#elif defined(_SC_PAGE_SIZE)
	return sysconf(_SC_PAGE_SIZE);
#else
	return 1;
#endif
}
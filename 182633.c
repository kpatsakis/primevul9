static int unix_detach(void *instance)
{
#define inst ((struct unix_instance *)instance)

	paircompare_unregister(PW_GROUP, groupcmp);
#ifdef PW_GROUP_NAME
	paircompare_unregister(PW_GROUP_NAME, groupcmp);
#endif
#undef inst
	free(instance);
	return 0;
}
static void hog_exit(void)
{
	if (suspend_supported)
		suspend_exit();

	btd_profile_unregister(&hog_profile);
}
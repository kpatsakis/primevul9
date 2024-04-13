static int hog_init(void)
{
	int err;

	err = suspend_init(suspend_callback, resume_callback);
	if (err < 0)
		error("Loading suspend plugin failed: %s (%d)", strerror(-err),
									-err);
	else
		suspend_supported = TRUE;

	return btd_profile_register(&hog_profile);
}
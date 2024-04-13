std::string get_cache_dir()
{
	if (cache_dir.empty())
	{
#if defined(_X11) && !defined(PREFERENCES_DIR)
		char const *xdg_cache = getenv("XDG_CACHE_HOME");
		if (!xdg_cache || xdg_cache[0] == '\0') {
			xdg_cache = getenv("HOME");
			if (!xdg_cache) {
				cache_dir = get_dir(get_user_data_dir() + "/cache");
				return cache_dir;
			}
			cache_dir = xdg_cache;
			cache_dir += "/.cache";
		} else cache_dir = xdg_cache;
		cache_dir += "/wesnoth";
		create_directory_if_missing_recursive(cache_dir);
#else
		cache_dir = get_dir(get_user_data_dir() + "/cache");
#endif
	}
	return cache_dir;
}
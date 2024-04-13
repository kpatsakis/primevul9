void set_user_data_dir(std::string path)
{
#ifdef _WIN32
	if(path.empty()) {
		user_data_dir = get_cwd() + "/userdata";
	} else if (path.size() > 2 && path[1] == ':') {
		//allow absolute path override
		user_data_dir = path;
	} else {
		typedef BOOL (WINAPI *SHGSFPAddress)(HWND, LPSTR, int, BOOL);
		SHGSFPAddress SHGetSpecialFolderPathA;
		HMODULE module = LoadLibraryA("shell32");
		SHGetSpecialFolderPathA = reinterpret_cast<SHGSFPAddress>(GetProcAddress(module, "SHGetSpecialFolderPathA"));
		if(SHGetSpecialFolderPathA) {
			LOG_FS << "Using SHGetSpecialFolderPath to find My Documents" << std::endl;
			char my_documents_path[MAX_PATH];
			if(SHGetSpecialFolderPathA(NULL, my_documents_path, 5, 1)) {
				std::string mygames_path = std::string(my_documents_path) + "/" + "My Games";
				boost::algorithm::replace_all(mygames_path, std::string("\\"), std::string("/"));
				create_directory_if_missing(mygames_path);
				user_data_dir = mygames_path + "/" + path;
			} else {
				WRN_FS << "SHGetSpecialFolderPath failed" << std::endl;
				user_data_dir = get_cwd() + "/" + path;
			}
		} else {
			LOG_FS << "Failed to load SHGetSpecialFolderPath function" << std::endl;
			user_data_dir = get_cwd() + "/" + path;
		}
	}

#else /*_WIN32*/

#ifdef PREFERENCES_DIR
	if (path.empty()) path = PREFERENCES_DIR;
#endif

	std::string path2 = ".wesnoth" + get_version_path_suffix();

#ifdef _X11
	const char *home_str = getenv("HOME");

	if (path.empty()) {
		char const *xdg_data = getenv("XDG_DATA_HOME");
		if (!xdg_data || xdg_data[0] == '\0') {
			if (!home_str) {
				path = path2;
				goto other;
			}
			user_data_dir = home_str;
			user_data_dir += "/.local/share";
		} else user_data_dir = xdg_data;
		user_data_dir += "/wesnoth/";
		user_data_dir += get_version_path_suffix();
		create_directory_if_missing_recursive(user_data_dir);
	} else {
		other:
		std::string home = home_str ? home_str : ".";

		if (path[0] == '/')
			user_data_dir = path;
		else
			user_data_dir = home + "/" + path;
	}
#else
	if (path.empty()) path = path2;

	const char* home_str = getenv("HOME");
	std::string home = home_str ? home_str : ".";

	if (path[0] == '/')
		user_data_dir = path;
	else
		user_data_dir = home + std::string("/") + path;
#endif

#endif /*_WIN32*/
	setup_user_data_dir();
}
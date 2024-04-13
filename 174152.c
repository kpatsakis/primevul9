void get_files_in_dir(const std::string &directory,
					  std::vector<std::string>* files,
					  std::vector<std::string>* dirs,
					  file_name_option mode,
					  file_filter_option filter,
					  file_reorder_option reorder,
					  file_tree_checksum* checksum)
{
	// If we have a path to find directories in,
	// then convert relative pathnames to be rooted
	// on the wesnoth path
	if(!directory.empty() && directory[0] != '/' && !game_config::path.empty()){
		std::string dir = game_config::path + "/" + directory;
		if(is_directory(dir)) {
			get_files_in_dir(dir,files,dirs,mode,filter,reorder,checksum);
			return;
		}
	}

	struct stat st;

	if (reorder == DO_REORDER) {
		LOG_FS << "searching for _main.cfg in directory " << directory << '\n';
		std::string maincfg;
		if (directory.empty() || directory[directory.size()-1] == '/')
			maincfg = directory + maincfg_filename;
		else
			maincfg = (directory + "/") + maincfg_filename;

		if (::stat(maincfg.c_str(), &st) != -1) {
			LOG_FS << "_main.cfg found : " << maincfg << '\n';
			if (files != NULL) {
				if (mode == ENTIRE_FILE_PATH)
					files->push_back(maincfg);
				else
					files->push_back(maincfg_filename);
			}
			return;
		}
	}

	DIR* dir = opendir(directory.c_str());

	if(dir == NULL) {
		// Probably not a directory, let the caller deal with it.
		return;
	}

	struct dirent* entry;
	while((entry = readdir(dir)) != NULL) {
		if(entry->d_name[0] == '.')
			continue;
#ifdef __APPLE__
		// HFS Mac OS X decomposes filenames using combining unicode characters.
		// Try to get the precomposed form.
		char macname[MAXNAMLEN+1];
		CFStringRef cstr = CFStringCreateWithCString(NULL,
							 entry->d_name,
							 kCFStringEncodingUTF8);
		CFMutableStringRef mut_str = CFStringCreateMutableCopy(NULL,
							 0, cstr);
		CFStringNormalize(mut_str, kCFStringNormalizationFormC);
		CFStringGetCString(mut_str,
				macname,sizeof(macname)-1,
				kCFStringEncodingUTF8);
		CFRelease(cstr);
		CFRelease(mut_str);
		const std::string basename = macname;
#else
		// generic Unix
		const std::string basename = entry->d_name;
#endif /* !APPLE */

		std::string fullname;
		if (directory.empty() || directory[directory.size()-1] == '/')
			fullname = directory + basename;
		else
			fullname = directory + "/" + basename;

		if (::stat(fullname.c_str(), &st) != -1) {
			if (S_ISREG(st.st_mode)) {
				if(filter == SKIP_PBL_FILES && looks_like_pbl(basename)) {
					continue;
				}
				if (files != NULL) {
					if (mode == ENTIRE_FILE_PATH)
						files->push_back(fullname);
					else
						files->push_back(basename);
				}
				if (checksum != NULL) {
					if(st.st_mtime > checksum->modified) {
						checksum->modified = st.st_mtime;
					}
					checksum->sum_size += st.st_size;
					checksum->nfiles++;
				}
			} else if (S_ISDIR(st.st_mode)) {
				if (filter == SKIP_MEDIA_DIR
						&& (basename == "images"|| basename == "sounds"))
					continue;

				if (reorder == DO_REORDER &&
						::stat((fullname+"/"+maincfg_filename).c_str(), &st)!=-1 &&
						S_ISREG(st.st_mode)) {
					LOG_FS << "_main.cfg found : ";
					if (files != NULL) {
						if (mode == ENTIRE_FILE_PATH) {
							files->push_back(fullname + "/" + maincfg_filename);
							LOG_FS << fullname << "/" << maincfg_filename << '\n';
						} else {
							files->push_back(basename + "/" + maincfg_filename);
							LOG_FS << basename << "/" << maincfg_filename << '\n';
					}
					} else {
					// Show what I consider strange
						LOG_FS << fullname << "/" << maincfg_filename << " not used now but skip the directory " << std::endl;
					}
				} else if (dirs != NULL) {
					if (mode == ENTIRE_FILE_PATH)
						dirs->push_back(fullname);
					else
						dirs->push_back(basename);
				}
			}
		}
	}

	closedir(dir);

	if(files != NULL)
		std::sort(files->begin(),files->end());

	if (dirs != NULL)
		std::sort(dirs->begin(),dirs->end());

	if (files != NULL && reorder == DO_REORDER) {
		// move finalcfg_filename, if present, to the end of the vector
		for (unsigned int i = 0; i < files->size(); i++) {
			if (ends_with((*files)[i], "/" + finalcfg_filename)) {
				files->push_back((*files)[i]);
				files->erase(files->begin()+i);
				break;
			}
		}
		// move initialcfg_filename, if present, to the beginning of the vector
		unsigned int foundit = 0;
		for (unsigned int i = 0; i < files->size(); i++)
			if (ends_with((*files)[i], "/" + initialcfg_filename)) {
				foundit = i;
				break;
			}
		// If _initial.cfg needs to be moved (it was found, but not at index 0).
		if (foundit > 0) {
			std::string initialcfg = (*files)[foundit];
			for (unsigned int i = foundit; i > 0; --i)
				(*files)[i] = (*files)[i-1];
			(*files)[0] = initialcfg;
		}
	}
}
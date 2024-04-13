static void push_if_exists(std::vector<std::string> *vec, const path &file, bool full) {
	if (vec != NULL) {
		if (full)
			vec->push_back(file.generic_string());
		else
			vec->push_back(file.filename().generic_string());
	}
}
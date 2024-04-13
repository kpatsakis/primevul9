std::string normalize_path(const std::string &p1)
{
	if (p1.empty()) return p1;

	std::string p2;
#ifdef _WIN32
	if (p1.size() >= 2 && p1[1] == ':')
		// Windows relative paths with explicit drive name are not handled.
		p2 = p1;
	else
#endif
	if (!is_path_sep(p1[0]))
		p2 = get_cwd() + "/" + p1;
	else
		p2 = p1;

#ifdef _WIN32
	std::string drive;
	if (p2.size() >= 2 && p2[1] == ':') {
		drive = p2.substr(0, 2);
		p2.erase(0, 2);
	}
#endif

	std::vector<std::string> components(1);
	for (int i = 0, i_end = p2.size(); i <= i_end; ++i)
	{
		std::string &last = components[components.size() - 1];
		char c = p2.c_str()[i];
		if (is_path_sep(c) || c == 0)
		{
			if (last == ".")
				last.clear();
			else if (last == "..")
			{
				if (components.size() >= 2) {
					components.pop_back();
					components[components.size() - 1].clear();
				} else
					last.clear();
			}
			else if (!last.empty())
				components.push_back(std::string());
		}
		else
			last += c;
	}

	std::ostringstream p4;
	components.pop_back();

#ifdef _WIN32
	p4 << drive;
#endif

	BOOST_FOREACH(const std::string &s, components)
	{
		p4 << '/' << s;
	}

	DBG_FS << "Normalizing '" << p2 << "' to '" << p4.str() << "'" << std::endl;

	return p4.str();
}
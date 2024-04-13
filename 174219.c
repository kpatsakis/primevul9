bool is_path_sep(char c)
{
	static const path sep = path("/").make_preferred();
	const std::string s = std::string(1, c);
	return sep == path(s).make_preferred();
}
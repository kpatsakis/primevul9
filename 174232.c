std::string read_file(const std::string &fname)
{
	scoped_istream is = istream_file(fname);
	std::stringstream ss;
	ss << is->rdbuf();
	return ss.str();
}
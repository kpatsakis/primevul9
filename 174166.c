std::istream *istream_file(const std::string &fname, bool treat_failure_as_error)
{
	LOG_FS << "Streaming " << fname << " for reading." << std::endl;
	if (fname.empty())
	{
		ERR_FS << "Trying to open file with empty name." << std::endl;
		std::ifstream *s = new std::ifstream();
		s->clear(std::ios_base::failbit);
		return s;
	}

	std::ifstream *s = new std::ifstream(fname.c_str(),std::ios_base::binary);
	if (s->is_open()) {
		return s;
	}

	if (treat_failure_as_error) {
		ERR_FS << "Could not open '" << fname << "' for reading." << std::endl;
	} else {
		LOG_FS << "Could not open '" << fname << "' for reading." << std::endl;
	}
	return s;
}
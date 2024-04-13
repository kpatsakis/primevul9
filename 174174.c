std::ostream *ostream_file(std::string const &fname)
{
	LOG_FS << "streaming " << fname << " for writing." << std::endl;
	return new std::ofstream(fname.c_str(), std::ios_base::binary);
}
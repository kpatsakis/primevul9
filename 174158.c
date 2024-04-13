std::ostream *ostream_file(std::string const &fname)
{
	LOG_FS << "streaming " << fname << " for writing.\n";
#if 1
	try
	{
		boost::iostreams::file_descriptor_sink fd(iostream_path(fname), std::ios_base::binary);
		return new boost::iostreams::stream<boost::iostreams::file_descriptor_sink>(fd, 4096, 0);
	}
	catch(BOOST_IOSTREAMS_FAILURE& e)
	{
		throw filesystem::io_exception(e.what());
	}
#else
	return new bfs::ofstream(path(fname), std::ios_base::binary);
#endif
}
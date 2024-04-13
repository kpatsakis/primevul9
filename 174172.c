void write_file(const std::string& fname, const std::string& data)
{
	scoped_ostream os = ostream_file(fname);
	os->exceptions(std::ios_base::goodbit);

	const size_t block_size = 4096;
	char buf[block_size];

	for(size_t i = 0; i < data.size(); i += block_size) {
		const size_t bytes = std::min<size_t>(block_size,data.size() - i);
		std::copy(data.begin() + i, data.begin() + i + bytes,buf);

		os->write(buf, bytes);
		if (os->bad())
			throw io_exception("Error writing to file: '" + fname + "'");
	}
}
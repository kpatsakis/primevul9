static inline bool error_except_not_found(const error_code &ec)
{
	return (ec
		&& ec.value() != boost::system::errc::no_such_file_or_directory
#ifdef _WIN32
		&& ec.value() != boost::system::windows_error::path_not_found
#endif /*_WIN32*/
		);
}
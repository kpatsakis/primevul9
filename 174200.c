bool create_directory_if_missing(const std::string& dirname)
{
	if(is_directory(dirname)) {
		DBG_FS << "directory " << dirname << " exists, not creating" << std::endl;
		return true;
	} else if(file_exists(dirname)) {
		ERR_FS << "cannot create directory " << dirname << "; file exists" << std::endl;
		return false;
	}
	DBG_FS << "creating missing directory " << dirname << '\n';
	return make_directory(dirname);
}
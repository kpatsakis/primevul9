static bool clear_ads(uint32_t create_disposition)
{
	bool ret = false;

	switch (create_disposition) {
	case FILE_SUPERSEDE:
	case FILE_OVERWRITE_IF:
	case FILE_OVERWRITE:
		ret = true;
		break;
	default:
		break;
	}
	return ret;
}
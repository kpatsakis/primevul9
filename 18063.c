static int disposition_to_open_flags(uint32_t create_disposition)
{
	int ret = 0;

	/*
	 * Currently we're using FILE_SUPERSEDE as the same as
	 * FILE_OVERWRITE_IF but they really are
	 * different. FILE_SUPERSEDE deletes an existing file
	 * (requiring delete access) then recreates it.
	 */

	switch (create_disposition) {
	case FILE_SUPERSEDE:
	case FILE_OVERWRITE_IF:
		/*
		 * If file exists replace/overwrite. If file doesn't
		 * exist create.
		 */
		ret = O_CREAT|O_TRUNC;
		break;

	case FILE_OPEN:
		/*
		 * If file exists open. If file doesn't exist error.
		 */
		ret = 0;
		break;

	case FILE_OVERWRITE:
		/*
		 * If file exists overwrite. If file doesn't exist
		 * error.
		 */
		ret = O_TRUNC;
		break;

	case FILE_CREATE:
		/*
		 * If file exists error. If file doesn't exist create.
		 */
		ret = O_CREAT|O_EXCL;
		break;

	case FILE_OPEN_IF:
		/*
		 * If file exists open. If file doesn't exist create.
		 */
		ret = O_CREAT;
		break;
	}
	return ret;
}
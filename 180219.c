ves_icall_System_IO_DriveInfo_GetDiskFreeSpace (MonoString *path_name, guint64 *free_bytes_avail,
						guint64 *total_number_of_bytes, guint64 *total_number_of_free_bytes,
						gint32 *error)
{
	gboolean result;
	ULARGE_INTEGER wapi_free_bytes_avail;
	ULARGE_INTEGER wapi_total_number_of_bytes;
	ULARGE_INTEGER wapi_total_number_of_free_bytes;

	MONO_ARCH_SAVE_REGS;

	*error = ERROR_SUCCESS;
	result = GetDiskFreeSpaceEx (mono_string_chars (path_name), &wapi_free_bytes_avail, &wapi_total_number_of_bytes,
				     &wapi_total_number_of_free_bytes);

	if (result) {
		*free_bytes_avail = wapi_free_bytes_avail.QuadPart;
		*total_number_of_bytes = wapi_total_number_of_bytes.QuadPart;
		*total_number_of_free_bytes = wapi_total_number_of_free_bytes.QuadPart;
	} else {
		*free_bytes_avail = 0;
		*total_number_of_bytes = 0;
		*total_number_of_free_bytes = 0;
		*error = GetLastError ();
	}

	return result;
}
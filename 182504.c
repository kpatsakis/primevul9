CWD_API int php_sys_stat_ex(const char *path, struct stat *buf, int lstat) /* {{{ */
{
	WIN32_FILE_ATTRIBUTE_DATA data;
	__int64 t;
	const size_t path_len = strlen(path);

	if (!GetFileAttributesEx(path, GetFileExInfoStandard, &data)) {
		return stat(path, buf);
	}

	if (path_len >= 1 && path[1] == ':') {
		if (path[0] >= 'A' && path[0] <= 'Z') {
			buf->st_dev = buf->st_rdev = path[0] - 'A';
		} else {
			buf->st_dev = buf->st_rdev = path[0] - 'a';
		}
	} else if (IS_UNC_PATH(path, path_len)) {
		buf->st_dev = buf->st_rdev = 0;
	} else {
		char  cur_path[MAXPATHLEN+1];
		DWORD len = sizeof(cur_path);
		char *tmp = cur_path;

		while(1) {
			DWORD r = GetCurrentDirectory(len, tmp);
			if (r < len) {
				if (tmp[1] == ':') {
					if (path[0] >= 'A' && path[0] <= 'Z') {
						buf->st_dev = buf->st_rdev = path[0] - 'A';
					} else {
						buf->st_dev = buf->st_rdev = path[0] - 'a';
					}
				} else {
					buf->st_dev = buf->st_rdev = -1;
				}
				break;
			} else if (!r) {
				buf->st_dev = buf->st_rdev = -1;
				break;
			} else {
				len = r+1;
				tmp = (char*)malloc(len);
			}
		}
		if (tmp != cur_path) {
			free(tmp);
		}
	}

	buf->st_uid = buf->st_gid = buf->st_ino = 0;

	if (lstat && data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
		/* File is a reparse point. Get the target */
		HANDLE hLink = NULL;
		REPARSE_DATA_BUFFER * pbuffer;
		unsigned int retlength = 0;
		TSRM_ALLOCA_FLAG(use_heap_large);

		hLink = CreateFile(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_OPEN_REPARSE_POINT|FILE_FLAG_BACKUP_SEMANTICS, NULL);
		if(hLink == INVALID_HANDLE_VALUE) {
			return -1;
		}

		pbuffer = (REPARSE_DATA_BUFFER *)tsrm_do_alloca(MAXIMUM_REPARSE_DATA_BUFFER_SIZE, use_heap_large);
		if(!DeviceIoControl(hLink, FSCTL_GET_REPARSE_POINT, NULL, 0, pbuffer,  MAXIMUM_REPARSE_DATA_BUFFER_SIZE, &retlength, NULL)) {
			tsrm_free_alloca(pbuffer, use_heap_large);
			CloseHandle(hLink);
			return -1;
		}

		CloseHandle(hLink);

		if(pbuffer->ReparseTag == IO_REPARSE_TAG_SYMLINK) {
			buf->st_mode = S_IFLNK;
			buf->st_mode |= (data.dwFileAttributes & FILE_ATTRIBUTE_READONLY) ? (S_IREAD|(S_IREAD>>3)|(S_IREAD>>6)) : (S_IREAD|(S_IREAD>>3)|(S_IREAD>>6)|S_IWRITE|(S_IWRITE>>3)|(S_IWRITE>>6));
		}

#if 0 /* Not used yet */
		else if(pbuffer->ReparseTag == IO_REPARSE_TAG_MOUNT_POINT) {
			buf->st_mode |=;
		}
#endif
		tsrm_free_alloca(pbuffer, use_heap_large);
	} else {
		buf->st_mode = (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? (S_IFDIR|S_IEXEC|(S_IEXEC>>3)|(S_IEXEC>>6)) : S_IFREG;
		buf->st_mode |= (data.dwFileAttributes & FILE_ATTRIBUTE_READONLY) ? (S_IREAD|(S_IREAD>>3)|(S_IREAD>>6)) : (S_IREAD|(S_IREAD>>3)|(S_IREAD>>6)|S_IWRITE|(S_IWRITE>>3)|(S_IWRITE>>6));
	}

	if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
		int len = strlen(path);

		if (path[len-4] == '.') {
			if (_memicmp(path+len-3, "exe", 3) == 0 ||
				_memicmp(path+len-3, "com", 3) == 0 ||
				_memicmp(path+len-3, "bat", 3) == 0 ||
				_memicmp(path+len-3, "cmd", 3) == 0) {
				buf->st_mode  |= (S_IEXEC|(S_IEXEC>>3)|(S_IEXEC>>6));
			}
		}
	}

	buf->st_nlink = 1;
	t = data.nFileSizeHigh;
	t = t << 32;
	t |= data.nFileSizeLow;
	buf->st_size = t;
	buf->st_atime = FileTimeToUnixTime(data.ftLastAccessTime);
	buf->st_ctime = FileTimeToUnixTime(data.ftCreationTime);
	buf->st_mtime = FileTimeToUnixTime(data.ftLastWriteTime);
	return 0;
}
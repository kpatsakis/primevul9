static int tsrm_realpath_r(char *path, int start, int len, int *ll, time_t *t, int use_realpath, int is_dir, int *link_is_dir TSRMLS_DC) /* {{{ */
{
	int i, j, save;
	int directory = 0;
#ifdef TSRM_WIN32
	WIN32_FIND_DATA data;
	HANDLE hFind;
	TSRM_ALLOCA_FLAG(use_heap_large)
#else
	struct stat st;
#endif
	realpath_cache_bucket *bucket;
	char *tmp;
	TSRM_ALLOCA_FLAG(use_heap)

	while (1) {
		if (len <= start) {
			if (link_is_dir) {
				*link_is_dir = 1;
			}
			return start;
		}

		i = len;
		while (i > start && !IS_SLASH(path[i-1])) {
			i--;
		}

		if (i == len ||
			(i == len - 1 && path[i] == '.')) {
			/* remove double slashes and '.' */
			len = i - 1;
			is_dir = 1;
			continue;
		} else if (i == len - 2 && path[i] == '.' && path[i+1] == '.') {
			/* remove '..' and previous directory */
			is_dir = 1;
			if (link_is_dir) {
				*link_is_dir = 1;
			}
			if (i - 1 <= start) {
				return start ? start : len;
			}
			j = tsrm_realpath_r(path, start, i-1, ll, t, use_realpath, 1, NULL TSRMLS_CC);
			if (j > start) {
				j--;
				while (j > start && !IS_SLASH(path[j])) {
					j--;
				}
				if (!start) {
					/* leading '..' must not be removed in case of relative path */
					if (j == 0 && path[0] == '.' && path[1] == '.' &&
							IS_SLASH(path[2])) {
						path[3] = '.';
						path[4] = '.';
						path[5] = DEFAULT_SLASH;
						j = 5;
					} else if (j > 0 &&
							path[j+1] == '.' && path[j+2] == '.' &&
							IS_SLASH(path[j+3])) {
						j += 4;
						path[j++] = '.';
						path[j++] = '.';
						path[j] = DEFAULT_SLASH;
					}
				}
			} else if (!start && !j) {
				/* leading '..' must not be removed in case of relative path */
				path[0] = '.';
				path[1] = '.';
				path[2] = DEFAULT_SLASH;
				j = 2;
			}
			return j;
		}

		path[len] = 0;

		save = (use_realpath != CWD_EXPAND);

		if (start && save && CWDG(realpath_cache_size_limit)) {
			/* cache lookup for absolute path */
			if (!*t) {
				*t = time(0);
			}
			if ((bucket = realpath_cache_find(path, len, *t TSRMLS_CC)) != NULL) {
				if (is_dir && !bucket->is_dir) {
					/* not a directory */
					return -1;
				} else {
					if (link_is_dir) {
						*link_is_dir = bucket->is_dir;
					}
					memcpy(path, bucket->realpath, bucket->realpath_len + 1);
					return bucket->realpath_len;
				}
			}
		}

#ifdef TSRM_WIN32
		if (save && (hFind = FindFirstFile(path, &data)) == INVALID_HANDLE_VALUE) {
			if (use_realpath == CWD_REALPATH) {
				/* file not found */
				return -1;
			}
			/* continue resolution anyway but don't save result in the cache */
			save = 0;
		}

		if (save) {
			FindClose(hFind);
		}

		tmp = tsrm_do_alloca(len+1, use_heap);
		memcpy(tmp, path, len+1);

		if(save &&
				!(IS_UNC_PATH(path, len) && len >= 3 && path[2] != '?') &&
				(data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)) {
			/* File is a reparse point. Get the target */
			HANDLE hLink = NULL;
			REPARSE_DATA_BUFFER * pbuffer;
			unsigned int retlength = 0;
			int bufindex = 0, isabsolute = 0;
			wchar_t * reparsetarget;
			BOOL isVolume = FALSE;
			char printname[MAX_PATH];
			char substitutename[MAX_PATH];
			int printname_len, substitutename_len;
			int substitutename_off = 0;

			if(++(*ll) > LINK_MAX) {
				return -1;
			}

			hLink = CreateFile(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_OPEN_REPARSE_POINT|FILE_FLAG_BACKUP_SEMANTICS, NULL);
			if(hLink == INVALID_HANDLE_VALUE) {
				return -1;
			}

			pbuffer = (REPARSE_DATA_BUFFER *)tsrm_do_alloca(MAXIMUM_REPARSE_DATA_BUFFER_SIZE, use_heap_large);
			if (pbuffer == NULL) {
				return -1;
			}
			if(!DeviceIoControl(hLink, FSCTL_GET_REPARSE_POINT, NULL, 0, pbuffer,  MAXIMUM_REPARSE_DATA_BUFFER_SIZE, &retlength, NULL)) {
				tsrm_free_alloca(pbuffer, use_heap_large);
				CloseHandle(hLink);
				return -1;
			}

			CloseHandle(hLink);

			if(pbuffer->ReparseTag == IO_REPARSE_TAG_SYMLINK) {
				reparsetarget = pbuffer->SymbolicLinkReparseBuffer.ReparseTarget;
				printname_len = pbuffer->MountPointReparseBuffer.PrintNameLength / sizeof(WCHAR);
				isabsolute = (pbuffer->SymbolicLinkReparseBuffer.Flags == 0) ? 1 : 0;
				if (!WideCharToMultiByte(CP_THREAD_ACP, 0,
					reparsetarget + pbuffer->MountPointReparseBuffer.PrintNameOffset  / sizeof(WCHAR),
					printname_len + 1,
					printname, MAX_PATH, NULL, NULL
				)) {
					tsrm_free_alloca(pbuffer, use_heap_large);
					return -1;
				};
				printname_len = pbuffer->MountPointReparseBuffer.PrintNameLength / sizeof(WCHAR);
				printname[printname_len] = 0;

				substitutename_len = pbuffer->MountPointReparseBuffer.SubstituteNameLength / sizeof(WCHAR);
				if (!WideCharToMultiByte(CP_THREAD_ACP, 0,
					reparsetarget + pbuffer->MountPointReparseBuffer.SubstituteNameOffset / sizeof(WCHAR),
					substitutename_len + 1,
					substitutename, MAX_PATH, NULL, NULL
				)) {
					tsrm_free_alloca(pbuffer, use_heap_large);
					return -1;
				};
				substitutename[substitutename_len] = 0;
			}
			else if(pbuffer->ReparseTag == IO_REPARSE_TAG_MOUNT_POINT) {
				isabsolute = 1;
				reparsetarget = pbuffer->MountPointReparseBuffer.ReparseTarget;
				printname_len = pbuffer->MountPointReparseBuffer.PrintNameLength / sizeof(WCHAR);
				if (!WideCharToMultiByte(CP_THREAD_ACP, 0,
					reparsetarget + pbuffer->MountPointReparseBuffer.PrintNameOffset  / sizeof(WCHAR),
					printname_len + 1,
					printname, MAX_PATH, NULL, NULL
				)) {
					tsrm_free_alloca(pbuffer, use_heap_large);
					return -1;
				};
				printname[pbuffer->MountPointReparseBuffer.PrintNameLength / sizeof(WCHAR)] = 0;

				substitutename_len = pbuffer->MountPointReparseBuffer.SubstituteNameLength / sizeof(WCHAR);
				if (!WideCharToMultiByte(CP_THREAD_ACP, 0,
					reparsetarget + pbuffer->MountPointReparseBuffer.SubstituteNameOffset / sizeof(WCHAR),
					substitutename_len + 1,
					substitutename, MAX_PATH, NULL, NULL
				)) {
					tsrm_free_alloca(pbuffer, use_heap_large);
					return -1;
				};
				substitutename[substitutename_len] = 0;
			}
			else if (pbuffer->ReparseTag == IO_REPARSE_TAG_DEDUP) {
				isabsolute = 1;
				memcpy(substitutename, path, len + 1);
				substitutename_len = len;
			} else {
				tsrm_free_alloca(pbuffer, use_heap_large);
				return -1;
			}

			if(isabsolute && substitutename_len > 4) {
				/* Do not resolve volumes (for now). A mounted point can
				   target a volume without a drive, it is not certain that
				   all IO functions we use in php and its deps support
				   path with volume GUID instead of the DOS way, like:
				   d:\test\mnt\foo
				   \\?\Volume{62d1c3f8-83b9-11de-b108-806e6f6e6963}\foo
				*/
				if (strncmp(substitutename, "\\??\\Volume{",11) == 0
					|| strncmp(substitutename, "\\\\?\\Volume{",11) == 0
					|| strncmp(substitutename, "\\??\\UNC\\", 8) == 0
					) {
					isVolume = TRUE;
					substitutename_off = 0;
				} else
					/* do not use the \??\ and \\?\ prefix*/
					if (strncmp(substitutename, "\\??\\", 4) == 0
						|| strncmp(substitutename, "\\\\?\\", 4) == 0) {
					substitutename_off = 4;
				}
			}

			if (!isVolume) {
				char * tmp2 = substitutename + substitutename_off;
				for(bufindex = 0; bufindex < (substitutename_len - substitutename_off); bufindex++) {
					*(path + bufindex) = *(tmp2 + bufindex);
				}

				*(path + bufindex) = 0;
				j = bufindex;
			} else {
				j = len;
			}


#if VIRTUAL_CWD_DEBUG
			fprintf(stderr, "reparse: print: %s ", printname);
			fprintf(stderr, "sub: %s ", substitutename);
			fprintf(stderr, "resolved: %s ", path);
#endif
			tsrm_free_alloca(pbuffer, use_heap_large);

			if(isabsolute == 1) {
				if (!((j == 3) && (path[1] == ':') && (path[2] == '\\'))) {
					/* use_realpath is 0 in the call below coz path is absolute*/
					j = tsrm_realpath_r(path, 0, j, ll, t, 0, is_dir, &directory TSRMLS_CC);
					if(j < 0) {
						tsrm_free_alloca(tmp, use_heap);
						return -1;
					}
				}
			}
			else {
				if(i + j >= MAXPATHLEN - 1) {
					tsrm_free_alloca(tmp, use_heap);
					return -1;
				}

				memmove(path+i, path, j+1);
				memcpy(path, tmp, i-1);
				path[i-1] = DEFAULT_SLASH;
				j  = tsrm_realpath_r(path, start, i + j, ll, t, use_realpath, is_dir, &directory TSRMLS_CC);
				if(j < 0) {
					tsrm_free_alloca(tmp, use_heap);
					return -1;
				}
			}
			directory = (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);

			if(link_is_dir) {
				*link_is_dir = directory;
			}
		}
		else {
			if (save) {
				directory = (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
				if (is_dir && !directory) {
					/* not a directory */
					return -1;
				}
			}

#elif defined(NETWARE)
		save = 0;
		tmp = tsrm_do_alloca(len+1, use_heap);
		memcpy(tmp, path, len+1);
#else
		if (save && php_sys_lstat(path, &st) < 0) {
			if (use_realpath == CWD_REALPATH) {
				/* file not found */
				return -1;
			}
			/* continue resolution anyway but don't save result in the cache */
			save = 0;
		}

		tmp = tsrm_do_alloca(len+1, use_heap);
		memcpy(tmp, path, len+1);

		if (save && S_ISLNK(st.st_mode)) {
			if (++(*ll) > LINK_MAX || (j = php_sys_readlink(tmp, path, MAXPATHLEN)) < 0) {
				/* too many links or broken symlinks */
				tsrm_free_alloca(tmp, use_heap);
				return -1;
			}
			path[j] = 0;
			if (IS_ABSOLUTE_PATH(path, j)) {
				j = tsrm_realpath_r(path, 1, j, ll, t, use_realpath, is_dir, &directory TSRMLS_CC);
				if (j < 0) {
					tsrm_free_alloca(tmp, use_heap);
					return -1;
				}
			} else {
				if (i + j >= MAXPATHLEN-1) {
					tsrm_free_alloca(tmp, use_heap);
					return -1; /* buffer overflow */
				}
				memmove(path+i, path, j+1);
				memcpy(path, tmp, i-1);
				path[i-1] = DEFAULT_SLASH;
				j = tsrm_realpath_r(path, start, i + j, ll, t, use_realpath, is_dir, &directory TSRMLS_CC);
				if (j < 0) {
					tsrm_free_alloca(tmp, use_heap);
					return -1;
				}
			}
			if (link_is_dir) {
				*link_is_dir = directory;
			}
		} else {
			if (save) {
				directory = S_ISDIR(st.st_mode);
				if (link_is_dir) {
					*link_is_dir = directory;
				}
				if (is_dir && !directory) {
					/* not a directory */
					tsrm_free_alloca(tmp, use_heap);
					return -1;
				}
			}
#endif
			if (i - 1 <= start) {
				j = start;
			} else {
				/* some leading directories may be unaccessable */
				j = tsrm_realpath_r(path, start, i-1, ll, t, save ? CWD_FILEPATH : use_realpath, 1, NULL TSRMLS_CC);
				if (j > start) {
					path[j++] = DEFAULT_SLASH;
				}
			}
#ifdef TSRM_WIN32
			if (j < 0 || j + len - i >= MAXPATHLEN-1) {
				tsrm_free_alloca(tmp, use_heap);
				return -1;
			}
			if (save) {
				i = strlen(data.cFileName);
				memcpy(path+j, data.cFileName, i+1);
				j += i;
			} else {
				/* use the original file or directory name as it wasn't found */
				memcpy(path+j, tmp+i, len-i+1);
				j += (len-i);
			}
		}
#else
			if (j < 0 || j + len - i >= MAXPATHLEN-1) {
				tsrm_free_alloca(tmp, use_heap);
				return -1;
			}
			memcpy(path+j, tmp+i, len-i+1);
			j += (len-i);
		}
#endif

		if (save && start && CWDG(realpath_cache_size_limit)) {
			/* save absolute path in the cache */
			realpath_cache_add(tmp, len, path, j, directory, *t TSRMLS_CC);
		}

		tsrm_free_alloca(tmp, use_heap);
		return j;
	}
}
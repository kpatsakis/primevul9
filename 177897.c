PS_GC_FUNC(files)
{
	PS_FILES_DATA;

	/* we don't perform any cleanup, if dirdepth is larger than 0.
	   we return SUCCESS, since all cleanup should be handled by
	   an external entity (i.e. find -ctime x | xargs rm) */

	if (data->dirdepth == 0) {
		*nrdels = ps_files_cleanup_dir(data->basedir, maxlifetime TSRMLS_CC);
	}

	return SUCCESS;
}
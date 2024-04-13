cli_wm_readdir(int fd)
{
	int done;
	off_t end_offset;
	unsigned char info_id;
	macro_info_t macro_info;
	vba_project_t *vba_project;
	mso_fib_t fib;

	if (!word_read_fib(fd, &fib))
		return NULL;

	if(fib.macro_len == 0) {
		cli_dbgmsg("wm_readdir: No macros detected\n");
		/* Must be clean */
		return NULL;
	}
	cli_dbgmsg("wm_readdir: macro offset: 0x%.4x\n", (int)fib.macro_offset);
	cli_dbgmsg("wm_readdir: macro len: 0x%.4x\n\n", (int)fib.macro_len);

	/* Go one past the start to ignore start_id */
	if (lseek(fd, fib.macro_offset + 1, SEEK_SET) != (off_t)(fib.macro_offset + 1)) {
		cli_dbgmsg("wm_readdir: lseek macro_offset failed\n");
		return NULL;
	}

	end_offset = fib.macro_offset + fib.macro_len;
	done = FALSE;
	memset(&macro_info, '\0', sizeof(macro_info));

	while((lseek(fd, 0, SEEK_CUR) < end_offset) && !done) {
		if (cli_readn(fd, &info_id, 1) != 1) {
			cli_dbgmsg("wm_readdir: read macro_info failed\n");
			break;
		}
		switch (info_id) {
			case 0x01:
				if(macro_info.count)
					free(macro_info.entries);
				word_read_macro_info(fd, &macro_info);
				done = TRUE;
				break;
			case 0x03:
				if(!word_skip_oxo3(fd))
					done = TRUE;
				break;
			case 0x05:
				if(!word_skip_menu_info(fd))
					done = TRUE;
				break;
			case 0x10:
				if(!word_skip_macro_extnames(fd))
					done = TRUE;
				break;
			case 0x11:
				if(!word_skip_macro_intnames(fd))
					done = TRUE;
				break;
			case 0x40:	/* end marker */
			case 0x12:	/* ??? */
				done = TRUE;
				break;
			default:
				cli_dbgmsg("wm_readdir: unknown type: 0x%x\n", info_id);
				done = TRUE;
		}
	}


	if(macro_info.count == 0)
		return NULL;

	vba_project = create_vba_project(macro_info.count, "", NULL);

	if(vba_project) {
		vba_project->length = (uint32_t *)cli_malloc(sizeof(uint32_t) *
				macro_info.count);
		vba_project->key = (unsigned char *)cli_malloc(sizeof(unsigned char) *
				macro_info.count);
		if((vba_project->length != NULL) &&
		   (vba_project->key != NULL)) {
			int i;
			const macro_entry_t *m = macro_info.entries;

			for(i = 0; i < macro_info.count; i++) {
				vba_project->offset[i] = m->offset;
				vba_project->length[i] = m->len;
				vba_project->key[i] = m->key;
				m++;
			}
		} else {
			free(vba_project->name);
			free(vba_project->colls);
			free(vba_project->dir);
			free(vba_project->offset);
			if(vba_project->length)
				free(vba_project->length);
			if(vba_project->key)
				free(vba_project->key);
			free(vba_project);
			vba_project = NULL;
		}
	}
	free(macro_info.entries);

	return vba_project;
}
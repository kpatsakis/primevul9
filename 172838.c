static void rcc_element(REgg *egg, char *str) {
	if (!egg || !str) {
		return;
	}
	REggEmit *e = egg->remit;
	char *ptr, *p = str + strlen (str);
	int inside = 0;
	int num, num2;
	int i;

	if (CTX) {
		if (egg->lang.slurp == '"') {
			if (egg->lang.mode == NORMAL) {
				if (!egg->lang.dstvar) {
					egg->lang.dstvar = strdup (".fix0");
				}
				rcc_pushstr (egg, str, 1);
			}
		} else {
			if (egg->lang.callname) {
				if (strstr (egg->lang.callname, "while") || strstr (egg->lang.callname, "if")) {
					egg->lang.conditionstr = strdup (str);
				}
			}
			egg->lang.nargs = 0;
			if (egg->lang.mode == GOTO) {
				egg->lang.mode = NORMAL;	// XXX
			}
			while (p-- != str) {
				if (*p == '"') {
					inside ^= 1;
				} else if (*p == ',' && !inside) {
					*p = '\0';
					p = (char *) skipspaces (p + 1);
					rcc_pusharg (egg, p);
				}
			}
			rcc_pusharg (egg, str);
		}
	} else {
		switch (egg->lang.mode) {
		case ALIAS:
			if (!egg->lang.dstvar) {
				eprintf ("does not set name or content for alias\n");
				break;
			}
			e->equ (egg, egg->lang.dstvar, str);
			if (egg->lang.nalias > 255) {
				eprintf ("global-buffer-overflow in aliases\n");
				break;
			}
			for (i = 0; i < egg->lang.nalias; i++) {
				if (!strcmp (egg->lang.dstvar, egg->lang.aliases[i].name)) {
					R_FREE (egg->lang.aliases[i].name);
					R_FREE (egg->lang.aliases[i].content);
					break;
				}
			}
			egg->lang.aliases[i].name = strdup (egg->lang.dstvar);
			egg->lang.aliases[i].content = strdup (str);
			egg->lang.nalias = (i == egg->lang.nalias)? (egg->lang.nalias + 1): egg->lang.nalias;
			// allow alias overwrite
			R_FREE (egg->lang.dstvar);
			egg->lang.mode = NORMAL;
			break;
		case SYSCALL:
			if (!egg->lang.dstvar) {
				eprintf ("does not set name or arg for syscall\n");
				break;
			}
			if (egg->lang.nsyscalls > 255) {
				eprintf ("global-buffer-overflow in syscalls\n");
				break;
			}
			// XXX the mem for name and arg are not freed - MEMLEAK
			R_FREE (egg->lang.syscalls[egg->lang.nsyscalls].name);
			R_FREE (egg->lang.syscalls[egg->lang.nsyscalls].arg);
			egg->lang.syscalls[egg->lang.nsyscalls].name = strdup (egg->lang.dstvar);
			egg->lang.syscalls[egg->lang.nsyscalls].arg = strdup (str);
			egg->lang.nsyscalls++;
			R_FREE (egg->lang.dstvar);
			break;
		case GOTO:
			egg->lang.elem[egg->lang.elem_n] = 0;
			e->jmp (egg, egg->lang.elem, 0);
			break;
		case INCLUDE:
			str = ptr = (char *) find_alias (egg, skipspaces (str));
			if (ptr) {
				if (strchr (ptr, '"')) {
					ptr = strchr (ptr, '"') + 1;
					if ((p = strchr (ptr, '"'))) {
						*p = '\x00';
					} else {
						eprintf ("loss back quote in include directory\n");
					}
					egg->lang.includedir = strdup (ptr);
				} else {
					eprintf ("wrong include syntax\n");
					// for must use string to symbolize directory
					egg->lang.includedir = NULL;
				}
			} else {
				egg->lang.includedir = NULL;
			}
			R_FREE (str);
			break;
		default:
			p = strchr (str, ',');
			if (p) {
				*p = '\0';
				num2 = atoi (p + 1);
			} else {
				num2 = 0;
			}
			num = atoi (str) + num2;
			egg->lang.stackframe = num;
			egg->lang.stackfixed = num2;
			if (egg->lang.mode != NAKED) {
				e->frame (egg, egg->lang.stackframe + egg->lang.stackfixed);
			}
		}
		egg->lang.elem[0] = 0;
		egg->lang.elem_n = 0;
	}
}
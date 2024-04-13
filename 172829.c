static void rcc_next(REgg *egg) {
	const char *ocn;
	REggEmit *e = egg->remit;
	char *str = NULL, *p, *ptr, buf[64];
	int i;

	if (egg->lang.setenviron) {
		egg->lang.elem[egg->lang.elem_n - 1] = 0;
		r_sys_setenv (egg->lang.setenviron, egg->lang.elem);
		R_FREE (egg->lang.setenviron);
		return;
	}
	if (egg->lang.includefile) {
		char *p, *q, *path;
		egg->lang.elem[egg->lang.elem_n - 1] = 0;
		path = find_include (egg->lang.includedir, egg->lang.includefile);
		if (!path) {
			eprintf ("Cannot find include file '%s'\n", egg->lang.elem);
			return;
		}
		R_FREE (egg->lang.includefile);
		R_FREE (egg->lang.includedir);
		rcc_reset_callname (egg);
		p = q = r_file_slurp (path, NULL);
		if (p) {
			int oline = ++(egg->lang.line);
			egg->lang.elem[0] = 0;	// TODO: this must be a separate function
			egg->lang.elem_n = 0;
			egg->lang.line = 0;
			for (; *p; p++) {
				r_egg_lang_parsechar (egg, *p);
			}
			free (q);
			egg->lang.line = oline;
		} else {
			eprintf ("Cannot find '%s'\n", path);
		}
		free (path);
		return;
	}
	egg->lang.docall = 1;
	if (egg->lang.callname) {
		if (!strcmp (egg->lang.callname, "goto")) {
			if (egg->lang.nargs != 1) {
				eprintf ("Invalid number of arguments for goto()\n");
				return;
			}
			e->jmp (egg, egg->lang.ctxpush[CTX], 0);
			rcc_reset_callname (egg);
			return;
		}
		if (!strcmp (egg->lang.callname, "break")) {
			e->trap (egg);
			rcc_reset_callname (egg);
			return;
		}
		ptr = strchr (egg->lang.callname, '=');
		if (ptr) {
			*ptr = '\0';
			// ocn = ptr+1; // what is the point of this?
		}
		ocn = skipspaces (egg->lang.callname);
		if (!ocn) {
			return;
		}
		str = r_egg_mkvar (egg, buf, ocn, 0);
		if (!str) {
			eprintf ("Cannot mkvar\n");
			return;
		}
		if (*ocn == '.') {
			e->call (egg, str, 1);
		}
		if (!strcmp (str, "while")) {
			char var[128];
			if (egg->lang.lastctxdelta >= 0) {
				exit (eprintf ("ERROR: Unsupported while syntax\n"));
			}
			sprintf (var, "__begin_%d_%d_%d\n", egg->lang.nfunctions, CTX, egg->lang.nestedi[CTX - 1]);
			e->while_end (egg, var);// get_frame_label (1));
#if 0
			eprintf ("------------------------------------------ lastctx: %d\n", egg->lang.lastctxdelta);
			// TODO: the pushvar is required for the if(){}while(); constructions
			// char *pushvar = egg->lang.ctxpush[context+egg->lang.nbrackets-1];
			/* TODO: support to compare more than one expression (LOGICAL OR) */
			rcc_printf ("  pop %%eax\n");
			rcc_printf ("  cmp $0, %%eax\n");	// XXX MUST SUPPORT != 0 COMPARE HERE
			/* TODO : Simplify!! */
			// if (pushvar)
			// printf("  push %s /* wihle push */\n", pushvar);
			if (egg->lang.lastctxdelta < 0) {
				rcc_printf ("  jnz %s\n", get_frame_label (1));
			} else {
				rcc_printf ("  jnz %s\n", get_frame_label (0));
			}
			// if (pushvar)
			// printf("  pop %%"R_AX" /* while pop */\n");
#endif
			egg->lang.nargs = 0;
		} else {
			for (i = 0; i < egg->lang.nsyscalls; i++) {
				if (!strcmp (str, egg->lang.syscalls[i].name)) {
					p = egg->lang.syscallbody;
					e->comment (egg, "set syscall args");
					e->syscall_args (egg, egg->lang.nargs);
					egg->lang.docall = 0;
					e->comment (egg, "syscall");
					r_egg_lang_parsechar (egg, '\n');	/* FIX parsing issue */
					if (p) {
						for (; *p; p++) {
							r_egg_lang_parsechar (egg, *p);
						}
					} else {
						char *q, *s = e->syscall (egg, egg->lang.nargs);
						if (s) {
							for (q = s; *q; q++) {
								r_egg_lang_parsechar (egg, *q);
							}
							free (s);
						} else {
							eprintf ("Cannot get @syscall payload\n");
						}
					}
					egg->lang.docall = 0;
					break;
				}
			}
			if (egg->lang.docall) {
				for (i = 0; i < egg->lang.ninlines; i++) {
					if (!strcmp (str, egg->lang.inlines[i].name)) {
						p = egg->lang.inlines[i].body;
						egg->lang.docall = 0;
						e->comment (egg, "inline");
						r_egg_lang_parsechar (egg, '\n');	/* FIX parsing issue */
						for (; *p; p++) {
							r_egg_lang_parsechar (egg, *p);
						}
						egg->lang.docall = 0;
						break;
					}
				}
			}
			if (egg->lang.docall) {
				e->comment (egg, "call in egg->lang.mode %d", egg->lang.mode);
				e->call (egg, str, 0);
			}
		}
		if (egg->lang.nargs > 0) {
			e->restore_stack (egg, egg->lang.nargs * e->size);
		}

// fixed by izhuer
		/*
		if (ocn) { // Used to call .var0()
		    // WTF? ocn mustn't be NULL here
		    // XXX: Probably buggy and wrong
		    *buf = 0;
		    free (str);
		    str = r_egg_mkvar (egg, buf, ocn, 0);
		    if (*buf)
		        e->get_result (egg, buf); // Why should get_result into ocn?
		    //else { eprintf("external symbol %s\n", ocn); }
		}
		*/

		/* store result of call */
		if (egg->lang.dstvar) {
			//if (egg->lang.mode != NAKED) {
			*buf = 0;
			free (str);
			str = r_egg_mkvar (egg, buf, egg->lang.dstvar, 0);
			if (*buf == 0) {
				eprintf ("Cannot resolve variable '%s'\n", egg->lang.dstvar);
			} else {
				e->get_result (egg, buf);
			}
			//}
			R_FREE (egg->lang.dstvar);
		}
		rcc_reset_callname (egg);
	} else {// handle mathop
		int vs = 'l';
		char type, *eq, *ptr = egg->lang.elem, *tmp;
		egg->lang.elem[egg->lang.elem_n] = '\0';
		ptr = (char *) skipspaces (ptr);
		if (*ptr) {
			eq = strchr (ptr, '=');
			if (eq) {
				char *p = (char *) skipspaces (ptr);
				vs = egg->lang.varsize;
				*buf = *eq = '\x00';
				e->mathop (egg, '=', vs, '$', "0", e->regs (egg, 1));
				// avoid situation that egg->lang.mathline starts with a single '-'
				egg->lang.mathline = strdup ((char *) skipspaces (eq + 1));
				tmp = egg->lang.mathline;
				rcc_mathop (egg, &tmp, 2);
				R_FREE (egg->lang.mathline);
				tmp = NULL;
				// following code block is too ugly, oh noes
				p = r_egg_mkvar (egg, buf, ptr, 0);
				if (is_var (p)) {
					char *q = r_egg_mkvar (egg, buf, p, 0);
					if (q) {
						free (p);
						p = q;
					}
					if (egg->lang.varxs == '*' || egg->lang.varxs == '&') {
						eprintf ("not support for *ptr in egg->lang.dstvar\n");
					}
					// XXX: Not support for pointer
					type = ' ';
				} else {
					type = '$';
				}
				e->mathop (egg, '=', vs, type, e->regs (egg, 1), p);
				free (p);
				/*
				    char str2[64], *p, ch = *(eq-1);
				    *eq = '\0';
				    eq = (char*) skipspaces (eq+1);
				    p = r_egg_mkvar (egg, str2, ptr, 0);
				    vs = egg->lang.varsize;
				    if (is_var (eq)) {
				        eq = r_egg_mkvar (egg, buf, eq, 0);
				        if (egg->lang.varxs=='*')
				            e->load (egg, eq, egg->lang.varsize);
				        else
				        // XXX this is a hack .. must be integrated with pusharg
				        if (egg->lang.varxs=='&')
				            e->load_ptr (egg, eq);
				        if (eq) {
				            R_FREE (eq);
				        }
				        type = ' ';
				    } else type = '$';
				    vs = 'l'; // XXX: add support for != 'l' size
				eprintf("Getting into e->mathop with ch: %c\n", ch);
				eprintf("Getting into e->mathop with vs: %c\n", vs);
				eprintf("Getting into e->mathop with type: %c\n", type);
				eprintf("Getting into e->mathop with eq: %s\n", eq);
				eprintf("Getting into e->mathop with p: %s\n", p);
				    e->mathop (egg, ch, vs, type, eq, p);
				    free(p);
				*/
			} else {
				if (!strcmp (ptr, "break")) {	// handle 'break;'
					e->trap (egg);
					rcc_reset_callname (egg);
				} else {
					e->mathop (egg, '=', vs, '$', ptr, NULL);
				}
			}
		}
	}
	free (str);
}
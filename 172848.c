R_API char *r_egg_mkvar(REgg *egg, char *out, const char *_str, int delta) {
	int i, len, qi;
	char *oldstr = NULL, *str = NULL, foo[32], *q, *ret = NULL;

	delta += egg->lang.stackfixed;	// XXX can be problematic
	if (!_str) {
		return NULL;	/* fix segfault, but not badparsing */
	}
	/* XXX memory leak */
	ret = str = oldstr = strdup (skipspaces (_str));
	// if (num || str[0]=='0') { sprintf(out, "$%d", num); ret = out; }
	if ((q = strchr (str, ':'))) {
		*q = '\0';
		qi = atoi (q + 1);
		egg->lang.varsize = (qi == 1)? 'b': 'l';
	} else {
		egg->lang.varsize = 'l';
	}
	if (*str == '*' || *str == '&') {
		egg->lang.varxs = *str;
		str++;
	} else {
		egg->lang.varxs = 0;
	}
	if (str[0] == '.') {
		REggEmit *e = egg->remit;
		if (!strncmp (str + 1, "ret", 3)) {
			strcpy (out, e->retvar);
		} else if (!strncmp (str + 1, "fix", 3)) {
			int idx = (int)r_num_math (NULL, str + 4) + delta + e->size;
			e->get_var (egg, 0, out, idx - egg->lang.stackfixed);
			// sprintf(out, "%d(%%"R_BP")", -(atoi(str+4)+delta+R_SZ-egg->lang.stackfixed));
		} else if (!strncmp (str + 1, "var", 3)) {
			int idx = (int)r_num_math (NULL, str + 4) + delta + e->size;
			e->get_var (egg, 0, out, idx);
			// sprintf(out, "%d(%%"R_BP")", -(atoi(str+4)+delta+R_SZ));
		} else if (!strncmp (str + 1, "rarg", 4)) {
			if (e->get_ar) {
				int idx = (int)r_num_math (NULL, str + 5);
				e->get_ar (egg, out, idx);
			}
		} else if (!strncmp (str + 1, "arg", 3)) {
			if (str[4]) {
				if (egg->lang.stackframe == 0) {
					e->get_var (egg, 1, out, 4);	// idx-4);
				} else {
					int idx = (int)r_num_math (NULL, str + 4) + delta + e->size;
					e->get_var (egg, 2, out, idx + 4);
				}
			} else {
				/* TODO: return size of syscall */
				if (egg->lang.callname) {
					for (i = 0; i < egg->lang.nsyscalls; i++) {
						if (!strcmp (egg->lang.syscalls[i].name, egg->lang.callname)) {
							free (oldstr);
							return strdup (egg->lang.syscalls[i].arg);
						}
					}
					eprintf ("Unknown arg for syscall '%s'\n", egg->lang.callname);
				} else {
					eprintf ("NO CALLNAME '%s'\n", egg->lang.callname);
				}
			}
		} else if (!strncmp (str + 1, "reg", 3)) {
			// XXX: can overflow if out is small
			if (egg->lang.attsyntax) {
				snprintf (out, 32, "%%%s", e->regs (egg, atoi (str + 4)));
			} else {
				snprintf (out, 32, "%s", e->regs (egg, atoi (str + 4)));
			}
		} else {
			out = str;	/* TODO: show error, invalid var name? */
			eprintf ("Something is really wrong\n");
		}
		ret = strdup (out);
		free (oldstr);
	} else if (*str == '"' || *str == '\'') {
		int mustfilter = *str == '"';
		/* TODO: check for room in egg->lang.stackfixed area */
		str++;
		len = strlen (str) - 1;
		if (!egg->lang.stackfixed || egg->lang.stackfixed < len) {
			eprintf ("WARNING: No room in the static stackframe! (%d must be %d)\n",
				egg->lang.stackfixed, len);
		}
		str[len] = '\0';
		snprintf (foo, sizeof (foo) - 1, ".fix%d", egg->lang.nargs * 16);	/* XXX FIX DELTA !!!1 */
		free (egg->lang.dstvar);
		egg->lang.dstvar = strdup (skipspaces (foo));
		rcc_pushstr (egg, str, mustfilter);
		ret = r_egg_mkvar (egg, out, foo, 0);
		free (oldstr);
	}
	return ret;
}
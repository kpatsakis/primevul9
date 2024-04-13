static struct SYMBOL *process_pscomment(struct SYMBOL *s)
{
	char w[32], *p, *q;
	int lock, voice;
	float h1;

	p = s->text + 2;		/* skip '%%' */
	q = p + strlen(p) - 5;
	lock = strncmp(q, " lock", 5) == 0;
	if (lock)
		*q = '\0'; 
	p = get_str(w, p, sizeof w);
	if (s->state == ABC_S_HEAD
	 && !check_header(s)) {
		error(1, s, "Cannot have %%%%%s in tune header", w);
		return s;
	}
	switch (w[0]) {
	case 'b':
		if (strcmp(w, "beginps") == 0
		 || strcmp(w, "beginsvg") == 0) {
			char use;

			if (w[5] == 'p') {
				if (strncmp(p, "svg", 3) == 0)
					use = 's';
				else if (strncmp(p, "nosvg", 5) == 0)
					use = 'p';
				else
					use = 'b';
			} else {
				use = 'g';
			}
			p = s->text + 2 + 7;
			while (*p != '\0' && *p != '\n')
				p++;
			if (*p == '\0')
				return s;		/* empty */
			ps_def(s, p + 1, use);
			return s;
		}
		if (strcmp(w, "begintext") == 0) {
			int job;

			if (s->state == ABC_S_TUNE) {
				gen_ly(1);
			} else if (s->state == ABC_S_GLOBAL) {
				if (epsf || !in_fname)
					return s;
			}
			p = s->text + 2 + 9;
			while (*p == ' ' || *p == '\t')
				p++;
			if (*p != '\n') {
				job = get_textopt(p);
				while (*p != '\0' && *p != '\n')
					p++;
				if (*p == '\0')
					return s;	/* empty */
			} else {
				job = cfmt.textoption;
			}
			if (job != T_SKIP) {
				p++;
				write_text(w, p, job);
			}
			return s;
		}
		if (strcmp(w, "break") == 0) {
			struct brk_s *brk;

			if (s->state != ABC_S_HEAD) {
				error(1, s, "%%%%%s ignored", w);
				return s;
			}
			if (*p == '\0')
				return s;
			for (;;) {
				brk = malloc(sizeof *brk);
				p = get_symsel(&brk->symsel, p);
				if (!p) {
					error(1, s, "Bad selection in %%%%%s", w);
					return s;
				}
				brk->next = brks;
				brks = brk;
				if (*p != ',' && *p != ' ')
					break;
				p++;
			}
			return s;
		}
		break;
	case 'c':
		if (strcmp(w, "center") == 0)
			goto center;
		if (strcmp(w, "clef") == 0) {
			if (s->state != ABC_S_GLOBAL)
				clef_def(s);
			return s;
		}
		if (strcmp(w, "clip") == 0) {
			if (!cur_tune_opts) {
				error(1, s, "%%%%%s not in %%%%tune sequence", w);
				return s;
			}

			/* %%clip <symbol selection> "-" <symbol selection> */
			if (*p != '-') {
				p = get_symsel(&clip_start, p);
				if (!p) {
					error(1, s, "Bad start in %%%%%s", w);
					return s;
				}
				if (*p != '-') {
					error(1, s, "Lack of '-' in %%%%%s", w);
					return s;
				}
			}
			p++;
			p = get_symsel(&clip_end, p);
			if (!p) {
				error(1, s, "Bad end in %%%%%s", w);
				return s;
			}
			if (clip_start.bar < 0)
				clip_start.bar = 0;
			if (clip_end.bar < clip_start.bar
			 || (clip_end.bar == clip_start.bar
			  && clip_end.time <= clip_start.time)) {
				clip_end.bar = (short unsigned) ~0 >> 1;
			}
			return s;
		}
		break;
	case 'd':
		if (strcmp(w, "deco") == 0) {
			deco_add(p);
			return s;
		}
		if (strcmp(w, "dynamic") == 0) {
			set_voice_param(curvoice, s->state, w, p);
			return s;
		}
		break;
	case 'E':
		if (strcmp(w, "EPS") == 0) {
			float x1, y1, x2, y2;
			FILE *fp;
			char fn[STRL1], line[STRL1];

			gen_ly(1);
			if (secure
			 || cfmt.textoption == T_SKIP)
				return s;
			get_str(line, p, sizeof line);
			if ((fp = open_file(line, "eps", fn)) == NULL) {
				error(1, s, "No such file: %s", line);
				return s;
			}

			/* get the bounding box */
			x1 = x2 = 0;
			while (fgets(line, sizeof line, fp)) {
				if (strncmp(line, "%%BoundingBox:", 14) == 0) {
					if (sscanf(&line[14], "%f %f %f %f",
						   &x1, &y1, &x2, &y2) == 4)
						break;
				}
			}
			fclose(fp);
			if (x1 == x2) {
				error(1, s, "No bounding box in '%s'", fn);
				return s;
			}
			if (cfmt.textoption == T_CENTER
			 || cfmt.textoption == T_RIGHT) {
				float lw;

				lw = ((cfmt.landscape ? cfmt.pageheight : cfmt.pagewidth)
					- cfmt.leftmargin - cfmt.rightmargin) / cfmt.scale;
				if (cfmt.textoption == T_CENTER)
					x1 += (lw - (x2 - x1)) * 0.5;
				else
					x1 += lw - (x2 - x1);
			}
			a2b("\001");	/* include file (must be the first after eob) */
			bskip(y2 - y1);
			a2b("%.2f %.2f%%%s\n", x1, -y1, fn);
			buffer_eob(0);
			return s;
		}
		break;
	case 'g':
		if (strcmp(w, "gchord") == 0
		 || strcmp(w, "gstemdir") == 0) {
			set_voice_param(curvoice, s->state, w, p);
			return s;
		}
		if (strcmp(w, "glyph") == 0) {
			if (!svg && epsf <= 1)
				glyph_add(p);
			return s;
		}
		break;
	case 'm':
		if (strcmp(w, "map") == 0) {
			get_map(p);
			return s;
		}
		if (strcmp(w, "maxsysstaffsep") == 0) {
			if (s->state != ABC_S_TUNE)
				break;
			parsys->voice[curvoice - voice_tb].maxsep = scan_u(p, 0);
			return s;
		}
		if (strcmp(w, "multicol") == 0) {
			float bposy;

			generate();
			if (strncmp(p, "start", 5) == 0) {
				if (!in_page)
					a2b("%%\n");	/* initialize the output */
				buffer_eob(0);
				bposy = get_bposy();
				multicol_max = multicol_start = bposy;
				lmarg = cfmt.leftmargin;
				rmarg = cfmt.rightmargin;
			} else if (strncmp(p, "new", 3) == 0) {
				if (multicol_start == 0) {
					error(1, s,
					      "%%%%%s new without start", w);
				} else {
					buffer_eob(0);
					bposy = get_bposy();
					if (bposy < multicol_start)
						bskip((bposy - multicol_start)
								/ cfmt.scale);
					if (bposy < multicol_max)
						multicol_max = bposy;
					cfmt.leftmargin = lmarg;
					cfmt.rightmargin = rmarg;
				}
			} else if (strncmp(p, "end", 3) == 0) {
				if (multicol_start == 0) {
					error(1, s,
					      "%%%%%s end without start", w);
				} else {
					buffer_eob(0);
					bposy = get_bposy();
					if (bposy > multicol_max)
						bskip((bposy - multicol_max)
								/ cfmt.scale);
					else
						a2b("%%\n");	/* force write_buffer */
					cfmt.leftmargin = lmarg;
					cfmt.rightmargin = rmarg;
					multicol_start = 0;
					buffer_eob(0);
					if (!info['X' - 'A']
					 && !epsf)
						write_buffer();
				}
			} else {
				error(1, s,
				      "Unknown keyword '%s' in %%%%%s", p, w);
			}
			return s;
		}
		break;
	case 'n':
		if (strcmp(w, "newpage") == 0) {
			if (epsf || !in_fname)
				return s;
			if (s->state == ABC_S_TUNE)
				generate();
			buffer_eob(0);
			write_buffer();
//			use_buffer = 0;
			if (isdigit((unsigned char) *p))
				pagenum = atoi(p);
			close_page();
			if (s->state == ABC_S_TUNE)
				bskip(cfmt.topspace);
			return s;
		}
		break;
	case 'p':
		if (strcmp(w, "pos") == 0) {	// %%pos <type> <position>
			p = get_str(w, p, sizeof w);
			set_voice_param(curvoice, s->state, w, p);
			return s;
		}
		if (strcmp(w, "ps") == 0
		 || strcmp(w, "postscript") == 0) {
			ps_def(s, p, 'b');
			return s;
		}
		break;
	case 'o':
		if (strcmp(w, "ornament") == 0) {
			set_voice_param(curvoice, s->state, w, p);
			return s;
		}
		break;
	case 'r':
		if (strcmp(w, "repbra") == 0) {
			if (s->state != ABC_S_TUNE)
				return s;
			curvoice->norepbra = strchr("0FfNn", *p)
						|| *p == '\0';
			return s;
		}
		if (strcmp(w, "repeat") == 0) {
			int n, k;

			if (s->state != ABC_S_TUNE)
				return s;
			if (!curvoice->last_sym) {
				error(1, s,
				      "%%%s cannot start a tune", w);
				return s;
			}
			if (*p == '\0') {
				n = 1;
				k = 1;
			} else {
				n = atoi(p);
				if (n < 1
				 || (curvoice->last_sym->type == BAR
					&& n > 2)) {
					error(1, s,
					      "Incorrect 1st value in %%%%%s", w);
					return s;
				}
				while (*p != '\0' && !isspace((unsigned char) *p))
					p++;
				while (isspace((unsigned char) *p))
					p++;
				if (*p == '\0') {
					k = 1;
				} else {
					k = atoi(p);
					if (k < 1) {
//					 || (curvoice->last_sym->type == BAR
//					  && n == 2
//					  && k > 1)) {
						error(1, s,
						      "Incorrect 2nd value in %%%%%s", w);
						return s;
					}
				}
			}
			s->aux = REPEAT;
			if (curvoice->last_sym->type == BAR)
				s->doty = n;
			else
				s->doty = -n;
			sym_link(s, FMTCHG);
			s->nohdi1 = k;
			s->text = NULL;
			return s;
		}
		break;
	case 's':
		if (strcmp(w, "setbarnb") == 0) {
			if (s->state == ABC_S_TUNE) {
				struct SYMBOL *s2;
				int n;

				n = atoi(p);
				for (s2 = s->abc_next; s2; s2 = s2->abc_next) {
					if (s2->abc_type == ABC_T_BAR) {
						s2->aux = n;
						break;
					}
				}
				return s;
			}
			strcpy(w, "measurefirst");
			break;
		}
		if (strcmp(w, "sep") == 0) {
			float h2, len, lwidth;

			if (s->state == ABC_S_TUNE) {
				gen_ly(0);
			} else if (s->state == ABC_S_GLOBAL) {
				if (epsf || !in_fname)
					return s;
			}
			lwidth = (cfmt.landscape ? cfmt.pageheight : cfmt.pagewidth)
				- cfmt.leftmargin - cfmt.rightmargin;
			h1 = h2 = len = 0;
			if (*p != '\0') {
				h1 = scan_u(p, 0);
				while (*p != '\0' && !isspace((unsigned char) *p))
					p++;
				while (isspace((unsigned char) *p))
					p++;
			}
			if (*p != '\0') {
				h2 = scan_u(p, 0);
				while (*p != '\0' && !isspace((unsigned char) *p))
					p++;
				while (isspace((unsigned char) *p))
					p++;
			}
			if (*p != '\0')
				len = scan_u(p, 0);
			if (h1 < 1)
				h1 = 0.5 CM;
			if (h2 < 1)
				h2 = h1;
			if (len < 1)
				len = 3.0 CM;
			bskip(h1);
			a2b("%.1f %.1f sep0\n",
			     len / cfmt.scale,
			     (lwidth - len) * 0.5 / cfmt.scale);
			bskip(h2);
			buffer_eob(0);
			return s;
		}
		if (strcmp(w, "staff") == 0) {
			int staff;

			if (s->state != ABC_S_TUNE)
				return s;
			if (*p == '+')
				staff = curvoice->cstaff + atoi(p + 1);
			else if (*p == '-')
				staff = curvoice->cstaff - atoi(p + 1);
			else
				staff = atoi(p) - 1;
			if ((unsigned) staff > (unsigned) nstaff) {
				error(1, s, "Bad staff in %%%%%s", w);
				return s;
			}
			curvoice->floating = 0;
			curvoice->cstaff = staff;
			return s;
		}
		if (strcmp(w, "staffbreak") == 0) {
			if (s->state != ABC_S_TUNE)
				return s;
			if (isdigit(*p)) {
				s->xmx = scan_u(p, 0);
				if (s->xmx < 0) {
					error(1, s, "Bad value in %%%%%s", w);
					return s;
				}
				if (p[strlen(p) - 1] == 'f')
					s->doty = 1;
			} else {
				s->xmx = 0.5 CM;
				if (*p == 'f')
					s->doty = 1;
			}
			sym_link(s, STBRK);
			return s;
		}
		if (strcmp(w, "stafflines") == 0) {
			if (isdigit((unsigned char) *p)) {
				switch (atoi(p)) {
				case 0: p = "..."; break;
				case 1: p = "..|"; break;
				case 2: p = ".||"; break;
				case 3: p = ".|||"; break;
				case 4: p = "||||"; break;
				case 5: p = "|||||"; break;
				case 6: p = "||||||"; break;
				case 7: p = "|||||||"; break;
				case 8: p = "||||||||"; break;
				default:
					error(1, s, "Bad number of lines");
					break;
				}
			} else {
				int l;

				l = strlen(p);
				q = p;
				p = getarena(l + 1);
				strcpy(p, q);
			}
			if (s->state != ABC_S_TUNE) {
				for (voice = 0; voice < MAXVOICE; voice++)
					voice_tb[voice].stafflines = p;
			} else {
				curvoice->stafflines = p;
			}
			return s;
		}
		if (strcmp(w, "staffscale") == 0) {
			char *q;
			float scale;

			scale = strtod(p, &q);
			if (scale < 0.3 || scale > 2
			 || (*q != '\0' && *q != ' ')) {
				error(1, s, "Bad value in %%%%%s", w);
				return s;
			}
			if (s->state != ABC_S_TUNE) {
				for (voice = 0; voice < MAXVOICE; voice++)
					voice_tb[voice].staffscale = scale;
			} else {
				curvoice->staffscale = scale;
			}
			return s;
		}
		if (strcmp(w, "staves") == 0
		 || strcmp(w, "score") == 0) {
			if (s->state == ABC_S_GLOBAL)
				return s;
			get_staves(s);
			return s;
		}
		if (strcmp(w, "stemdir") == 0) {
			set_voice_param(curvoice, s->state, w, p);
			return s;
		}
		if (strcmp(w, "sysstaffsep") == 0) {
			if (s->state != ABC_S_TUNE)
				break;
			parsys->voice[curvoice - voice_tb].sep = scan_u(p, 0);
			return s;
		}
		break;
	case 't':
		if (strcmp(w, "text") == 0) {
			int job;

center:
			if (s->state == ABC_S_TUNE) {
				gen_ly(1);
			} else if (s->state == ABC_S_GLOBAL) {
				if (epsf || !in_fname)
					return s;
			}
			if (w[0] == 'c') {
				job = T_CENTER;
			} else {
				job = cfmt.textoption;
				switch(job) {
				case T_SKIP:
					return s;
				case T_LEFT:
				case T_RIGHT:
				case T_CENTER:
					break;
				default:
					job = T_LEFT;
					break;
				}
			}
			write_text(w, p, job);
			return s;
		}
		if (strcmp(w, "tablature") == 0) {
			struct tblt_s *tblt;
			int i, j;

			tblt = tblt_parse(p);
			if (tblt == 0)
				return s;

			switch (s->state) {
			case ABC_S_TUNE:
			case ABC_S_HEAD:
				for (i = 0; i < ncmdtblt; i++) {
					if (cmdtblts[i].active)
						continue;
					j = cmdtblts[i].index;
					if (j < 0 || tblts[j] == tblt)
						return s;
				}
				/* !! 2 tblts per voice !! */
				if (curvoice->tblts[0] == tblt
				 || curvoice->tblts[1] == tblt)
					break;
				if (curvoice->tblts[1]) {
					error(1, s,
						"Too many tablatures for voice %s",
						curvoice->id);
					break;
				}
				if (!curvoice->tblts[0])
					curvoice->tblts[0] = tblt;
				else
					curvoice->tblts[1] = tblt;
				break;
			}
			return s;
		}
		if (strcmp(w, "transpose") == 0) {
			struct VOICE_S *p_voice;
			struct SYMBOL *s2;
			int i, val;

			val = get_transpose(p);
			switch (s->state) {
			case ABC_S_GLOBAL:
				cfmt.transpose = val;
				return s;
			case ABC_S_HEAD: {
				cfmt.transpose += val;
				for (i = MAXVOICE, p_voice = voice_tb;
				     --i >= 0;
				     p_voice++) {
					p_voice->transpose = cfmt.transpose;
					memcpy(&p_voice->key, &p_voice->okey,
						sizeof p_voice->key);
					key_transpose(&p_voice->key);
					memcpy(&p_voice->ckey, &p_voice->key,
						sizeof p_voice->ckey);
					if (p_voice->key.empty)
						p_voice->key.sf = 0;
				}
				return s;
			    }
			}
			curvoice->transpose = cfmt.transpose + val;
			s2 = curvoice->sym;
			if (!s2) {
				memcpy(&curvoice->key, &curvoice->okey,
					sizeof curvoice->key);
				key_transpose(&curvoice->key);
				memcpy(&curvoice->ckey, &curvoice->key,
					sizeof curvoice->ckey);
				if (curvoice->key.empty)
					curvoice->key.sf = 0;
				return s;
			}
			for (;;) {
				if (s2->type == KEYSIG)
					break;
				if (s2->time == curvoice->time) {
					s2 = s2->prev;
					if (s2)
						continue;
				}
				s2 = s;
				s2->abc_type = ABC_T_INFO;
				s2->text = (char *) getarena(2);
				s2->text[0] = 'K';
				s2->text[1] = '\0';
				sym_link(s2, KEYSIG);
//				if (!curvoice->ckey.empty)
//					s2->aux = curvoice->ckey.sf;
				s2->aux = curvoice->key.sf;
				break;
			}
			memcpy(&s2->u.key, &curvoice->okey,
						sizeof s2->u.key);
			key_transpose(&s2->u.key);
			memcpy(&curvoice->ckey, &s2->u.key,
						sizeof curvoice->ckey);
			if (curvoice->key.empty)
				s2->u.key.sf = 0;
			return s;
		}
		if (strcmp(w, "tune") == 0) {
			struct SYMBOL *s2, *s3;
			struct tune_opt_s *opt, *opt2;

			if (s->state != ABC_S_GLOBAL) {
				error(1, s, "%%%%%s ignored", w);
				return s;
			}

			/* if void %%tune, remove all tune options */
			if (*p == '\0') {
				opt = tune_opts;
				while (opt) {
					free_voice_opt(opt->voice_opts);
					opt2 = opt->next;
					free(opt);
					opt = opt2;
				}
				tune_opts = NULL;
				return s;
			}

			if (strcmp(p, "end") == 0)
				return s;	/* end of previous %%tune */

			/* search the end of the tune options */
			s2 = s;
			for (;;) {
				s3 = s2->abc_next;
				if (!s3)
					break;
				if (s3->abc_type != ABC_T_NULL
				 && (s3->abc_type != ABC_T_PSCOM
				  || strncmp(&s3->text[2], "tune ", 5) == 0))
					break;
				s2 = s3;
			}

			/* search if already a same %%tune */
			opt2 = NULL;
			for (opt = tune_opts; opt; opt = opt->next) {
				if (strcmp(opt->s->text, s->text) == 0)
					break;
				opt2 = opt;
			}

			if (opt) {
				free_voice_opt(opt->voice_opts);
				if (s2 == s) {			/* no option */
					if (!opt2)
						tune_opts = opt->next;
					else
						opt2->next = opt->next;
					free(opt);
					return s;
				}
				opt->voice_opts = NULL;
			} else {
				if (s2 == s)			/* no option */
					return s;
				opt = malloc(sizeof *opt);
				memset(opt, 0, sizeof *opt);
				opt->next = tune_opts;
				tune_opts = opt;
			}

			/* link the options */
			opt->s = s3 = s;
			cur_tune_opts = opt;
			s = s->abc_next;
			for (;;) {
				if (s->abc_type != ABC_T_PSCOM)
					continue;
				if (strncmp(&s->text[2], "voice ", 6) == 0) {
					s = process_pscomment(s);
				} else {
					s->state = ABC_S_HEAD;

					/* !! no reverse link !! */
					s3->next = s;
					s3 = s;
				}
				if (s == s2)
					break;
				s = s->abc_next;
			}
			cur_tune_opts = NULL;
			return s;
		}
		break;
	case 'u':
		if (strcmp(w, "user") == 0) {
			deco[s->u.user.symbol] = parse.deco_tb[s->u.user.value - 128];
			return s;
		}
		break;
	case 'v':
		if (strcmp(w, "vocal") == 0) {
			set_voice_param(curvoice, s->state, w, p);
			return s;
		}
		if (strcmp(w, "voice") == 0) {
			struct SYMBOL *s2, *s3;
			struct voice_opt_s *opt, *opt2;

			if (s->state != ABC_S_GLOBAL) {
				error(1, s, "%%%%voice ignored");
				return s;
			}

			/* if void %%voice, free all voice options */
			if (*p == '\0') {
				if (cur_tune_opts) {
					free_voice_opt(cur_tune_opts->voice_opts);
					cur_tune_opts->voice_opts = NULL;
				} else {
					free_voice_opt(voice_opts);
					voice_opts = NULL;
				}
				return s;
			}

			if (strcmp(p, "end") == 0)
				return s;	/* end of previous %%voice */

			if (cur_tune_opts)
				opt = cur_tune_opts->voice_opts;
			else
				opt = voice_opts;

			/* search the end of the voice options */
			s2 = s;
			for (;;) {
				s3 = s2->abc_next;
				if (!s3)
					break;
				if (s3->abc_type != ABC_T_NULL
				 && (s3->abc_type != ABC_T_PSCOM
				  || strncmp(&s3->text[2], "score ", 6) == 0
				  || strncmp(&s3->text[2], "staves ", 7) == 0
				  || strncmp(&s3->text[2], "tune ", 5) == 0
				  || strncmp(&s3->text[2], "voice ", 6) == 0))
					break;
				s2 = s3;
			}

			/* if already the same %%voice
			 * remove the options */
			opt2 = NULL;
			for ( ; opt; opt = opt->next) {
				if (strcmp(opt->s->text, s->text) == 0) {
					if (!opt2) {
						if (cur_tune_opts)
							cur_tune_opts->voice_opts = NULL;
						else
							voice_opts = NULL;
					} else {
						opt2->next = opt->next;
					}
					free(opt);
					break;
				}
				opt2 = opt;
			}
			if (s2 == s)		/* no option */
				return s;
			opt = malloc(sizeof *opt + strlen(p));
			memset(opt, 0, sizeof *opt);
			if (cur_tune_opts) {
				opt->next = cur_tune_opts->voice_opts;
				cur_tune_opts->voice_opts = opt;
			} else {
				opt->next = voice_opts;
				voice_opts = opt;
			}

			/* link the options */
			opt->s = s3 = s;
			for ( ; s != s2; s = s->abc_next) {
				if (s->abc_next->abc_type != ABC_T_PSCOM)
					continue;
				s->abc_next->state = ABC_S_TUNE;
				s3->next = s->abc_next;
				s3 = s3->next;
			}
			return s;
		}
		if (strcmp(w, "voicecolor") == 0) {
			int color;

			if (!curvoice)
				return s;

			color = get_color(p);
			if (color < 0)
				error(1, s, "Bad color in %%%%voicecolor");
			else
				curvoice->color = color;
			return s;
		}
		if (strcmp(w, "voicecombine") == 0) {
			int combine;

			if (sscanf(p, "%d", &combine) != 1) {
				error(1, s, "Bad value in %%%%voicecombine");
				return s;
			}
			switch (s->state) {
			case ABC_S_GLOBAL:
				cfmt.combinevoices = combine;
				break;
			case ABC_S_HEAD:
				for (voice = 0; voice < MAXVOICE; voice++)
					voice_tb[voice].combine = combine;
				break;
			default:
				curvoice->combine = combine;
				break;
			}
			return s;
		}
		if (strcmp(w, "voicemap") == 0) {
			if (s->state != ABC_S_TUNE) {
				for (voice = 0; voice < MAXVOICE; voice++)
					voice_tb[voice].map_name = p;
			} else {
				curvoice->map_name = p;
			}
			return s;
		}
		if (strcmp(w, "voicescale") == 0) {
			char *q;
			float scale;

			scale = strtod(p, &q);
			if (scale < 0.6 || scale > 1.5
			 || (*q != '\0' && *q != ' ')) {
				error(1, s, "Bad %%%%voicescale value");
				return s;
			}
			if (s->state != ABC_S_TUNE) {
				for (voice = 0; voice < MAXVOICE; voice++)
					voice_tb[voice].scale = scale;
			} else {
				curvoice->scale = scale;
			}
			return s;
		}
		if (strcmp(w, "volume") == 0) {
			set_voice_param(curvoice, s->state, w, p);
			return s;
		}
		if (strcmp(w, "vskip") == 0) {
			if (s->state == ABC_S_TUNE) {
				gen_ly(0);
			} else if (s->state == ABC_S_GLOBAL) {
				if (epsf || !in_fname)
					return s;
			}
			bskip(scan_u(p, 0));
			buffer_eob(0);
			return s;
		}
		break;
	}
	if (s->state == ABC_S_TUNE) {
		if (strcmp(w, "leftmargin") == 0
		 || strcmp(w, "rightmargin") == 0
		 || strcmp(w, "scale") == 0) {
			generate();
			block_put();
		}
	}
	interpret_fmt_line(w, p, lock);
	if (cfmt.alignbars && strcmp(w, "alignbars") == 0) {
		int i;

		generate();
		if ((unsigned) cfmt.alignbars > MAXSTAFF) {
			error(1, s, "Too big value in %%%%alignbars");
			cfmt.alignbars = MAXSTAFF;
		}
		if (staves_found >= 0)		/* (compatibility) */
			cfmt.alignbars = nstaff + 1;
		first_voice = curvoice = voice_tb;
		for (i = 0; i < cfmt.alignbars; i++) {
			voice_tb[i].staff = voice_tb[i].cstaff = i;
			voice_tb[i].next = &voice_tb[i + 1];
			parsys->staff[i].flags |= STOP_BAR;
			parsys->voice[i].staff = i;
			parsys->voice[i].range = i;
		}
		i--;
		voice_tb[i].next = NULL;
		parsys->nstaff = nstaff = i;
	}
	return s;
}
static inline void mb_regex_substitute(
	smart_str *pbuf,
	const char *subject,
	size_t subject_len,
	char *replace,
	size_t replace_len,
	php_mb_regex_t *regexp,
	OnigRegion *regs,
	const mbfl_encoding *enc
) {
	char *p, *sp, *eos;
	int no; /* bakreference group number */
	int clen; /* byte-length of the current character */

	p = replace;
	eos = replace + replace_len;

	while (p < eos) {
		clen = (int) php_mb_mbchar_bytes_ex(p, enc);
		if (clen != 1 || p == eos || p[0] != '\\') {
			/* skip anything that's not an ascii backslash */
			smart_str_appendl(pbuf, p, clen);
			p += clen;
			continue;
		}
		sp = p; /* save position */
		clen = (int) php_mb_mbchar_bytes_ex(++p, enc);
		if (clen != 1 || p == eos) {
			/* skip escaped multibyte char */
			p += clen;
			smart_str_appendl(pbuf, sp, p - sp);
			continue;
		}
		no = -1;
		switch (p[0]) {
			case '0':
				no = 0;
				p++;
				break;
			case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
				if (!onig_noname_group_capture_is_active(regexp)) {
					/*
					 * FIXME:
					 * Oniguruma throws a compile error if numbered backrefs are used with named groups in the pattern.
					 * For now we just ignore them, but in the future we might want to raise a warning
					 * and abort the whole replace operation.
					 */
					p++;
					smart_str_appendl(pbuf, sp, p - sp);
					continue;
				}
				no = p[0] - '0';
				p++;
				break;
			case 'k':
				clen = (int) php_mb_mbchar_bytes_ex(++p, enc);
				if (clen != 1 || p == eos || (p[0] != '<' && p[0] != '\'')) {
					/* not a backref delimiter */
					p += clen;
					smart_str_appendl(pbuf, sp, p - sp);
					continue;
				}
				/* try to consume everything until next delimiter */
				char delim = p[0] == '<' ? '>' : '\'';
				char *name, *name_end;
				char maybe_num = 1;
				name_end = name = p + 1;
				while (name_end < eos) {
					clen = (int) php_mb_mbchar_bytes_ex(name_end, enc);
					if (clen != 1) {
						name_end += clen;
						maybe_num = 0;
						continue;
					}
					if (name_end[0] == delim) break;
					if (maybe_num && !isdigit(name_end[0])) maybe_num = 0;
					name_end++;
				}
				p = name_end + 1;
				if (name_end - name < 1 || name_end >= eos) {
					/* the backref was empty or we failed to find the end delimiter */
					smart_str_appendl(pbuf, sp, p - sp);
					continue;
				}
				/* we have either a name or a number */
				if (maybe_num) {
					if (!onig_noname_group_capture_is_active(regexp)) {
						/* see above note on mixing numbered & named backrefs */
						smart_str_appendl(pbuf, sp, p - sp);
						continue;
					}
					if (name_end - name == 1) {
						no = name[0] - '0';
						break;
					}
					if (name[0] == '0') {
						/* 01 is not a valid number */
						break;
					}
					no = (int) strtoul(name, NULL, 10);
					break;
				}
				no = onig_name_to_backref_number(regexp, (OnigUChar *)name, (OnigUChar *)name_end, regs);
				break;
			default:
				p += clen;
				smart_str_appendl(pbuf, sp, p - sp);
				continue;
		}
		if (no < 0 || no >= regs->num_regs) {
			/* invalid group number reference, keep the escape sequence in the output */
			smart_str_appendl(pbuf, sp, p - sp);
			continue;
		}
		if (regs->beg[no] >= 0 && regs->beg[no] < regs->end[no] && (size_t)regs->end[no] <= subject_len) {
			smart_str_appendl(pbuf, subject + regs->beg[no], regs->end[no] - regs->beg[no]);
		}
	}

	if (p < eos) {
		smart_str_appendl(pbuf, p, eos - p);
	}
}
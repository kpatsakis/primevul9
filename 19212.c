R_API int r_core_anal_data(RCore *core, ut64 addr, int count, int depth, int wordsize) {
	RAnalData *d;
	ut64 dstaddr = 0LL;
	ut8 *buf = core->block;
	int len = core->blocksize;
	int word = wordsize ? wordsize: core->rasm->bits / 8;
	char *str;
	int i, j;

	count = R_MIN (count, len);
	buf = malloc (len + 1);
	if (!buf) {
		return false;
	}
	memset (buf, 0xff, len);
	r_io_read_at (core->io, addr, buf, len);
	buf[len - 1] = 0;

	RConsPrintablePalette *pal = r_config_get_i (core->config, "scr.color")? &r_cons_context ()->pal: NULL;
	for (i = j = 0; j < count; j++) {
		if (i >= len) {
			r_io_read_at (core->io, addr + i, buf, len);
			buf[len] = 0;
			addr += i;
			i = 0;
			continue;
		}
		/* r_anal_data requires null-terminated buffer according to coverity */
		/* but it should not.. so this must be fixed in anal/data.c instead of */
		/* null terminating here */
		d = r_anal_data (core->anal, addr + i, buf + i, len - i, wordsize);
		str = r_anal_data_to_string (d, pal);
		r_cons_println (str);

		if (d) {
			switch (d->type) {
			case R_ANAL_DATA_TYPE_POINTER:
				r_cons_printf ("`- ");
				dstaddr = r_mem_get_num (buf + i, word);
				if (depth > 0) {
					r_core_anal_data (core, dstaddr, 1, depth - 1, wordsize);
				}
				i += word;
				break;
			case R_ANAL_DATA_TYPE_STRING:
				buf[len-1] = 0;
				i += strlen ((const char*)buf + i) + 1;
				break;
			default:
				i += (d->len > 3)? d->len: word;
				break;
			}
		} else {
			i += word;
		}
		free (str);
		r_anal_data_free (d);
	}
	free (buf);
	return true;
}
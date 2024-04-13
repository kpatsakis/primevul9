fill_indent(PInfo pi, char *buf, size_t size) {
    size_t	cnt;

    if (0 < (cnt = helper_stack_depth(&pi->helpers))) {
	cnt *= 2;
	if (size < cnt + 1) {
	    cnt = size - 1;
	}
	memset(buf, ' ', cnt);
	buf += cnt;
    }
    *buf = '\0';
}
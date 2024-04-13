static void rcc_mathop(REgg *egg, char **pos, int level) {
	REggEmit *e = egg->remit;
	int op_ret = level;
	char op, *next_pos;

	while (**pos && is_space (**pos)) {
		(*pos)++;
	}
	next_pos = *pos + 1;

	do {
		op = (is_op (**pos) && !(is_var (*pos)))? **pos: '=';
		*pos = (is_op (**pos) && !(is_var (*pos)))? *pos + 1: *pos;
		op_ret = get_op (&next_pos);
		if (op_ret > level) {
			rcc_mathop (egg, pos, op_ret);
			rcc_internal_mathop (egg, e->regs (egg, op_ret - 1),
				strdup (e->regs (egg, level - 1)), op);
			next_pos = *pos + 1;
		} else {
			rcc_internal_mathop (egg, *pos, strdup (e->regs (egg, level - 1)), op);
			*pos = next_pos;
			next_pos++;
		}
	} while (**pos && op_ret >= level);

/* following code block sould not handle '-' and '/' well
    if (op_ret < level) {
        rcc_internal_mathop(egg, p, strdup(e->regs(egg, level-1)) ,'=');
        return;
    }
    op = *pos, *pos = '\x00', (*pos)++;
    rcc_mathop(egg, pos, op_ret);
    if (op_ret > level) {
        rcc_internal_mathop(egg, p, strdup(e->regs(egg, op_ret-1)), op);
        rcc_internal_mathop(egg, (char *)e->regs(egg, op_ret-1)
                            , strdup(e->regs(egg, level-1)), '=');
    }
    else rcc_internal_mathop(egg, p, strdup(e->regs(egg, level-1)), op);
*/
}
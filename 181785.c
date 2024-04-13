static void __core_anal_appcall(RCore *core, const char *input) {
//	r_reg_arena_push (core->dbg->reg);
	RListIter *iter;
	char *arg;
	char *inp = strdup (input);
	RList *args = r_str_split_list (inp, " ", 0);
	int i = 0;
	r_list_foreach (args, iter, arg) {
		const char *alias = sdb_fmt ("A%d", i);
		r_reg_setv (core->anal->reg, alias, r_num_math (core->num, arg));
		i++;
	}
	ut64 sp = r_reg_getv (core->anal->reg, "SP");
	r_reg_setv (core->anal->reg, "SP", 0);

	r_reg_setv (core->anal->reg, "PC", core->offset);
	r_core_cmd0 (core, "aesu 0");

	r_reg_setv (core->anal->reg, "SP", sp);
	free (inp);

//	r_reg_arena_pop (core->dbg->reg);
}
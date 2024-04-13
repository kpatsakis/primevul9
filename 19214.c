static char *get_function_name(RCore *core, ut64 addr) {
	RBinFile *bf = r_bin_cur (core->bin);
	if (bf && bf->o) {
		RBinSymbol *sym = ht_up_find (bf->o->addr2klassmethod, addr, NULL);
		if (sym && sym->classname && sym->name) {
			return r_str_newf ("method.%s.%s", sym->classname, sym->name);
		}
	}
	RFlagItem *flag = r_core_flag_get_by_spaces (core->flags, addr);
	return (flag && flag->name) ? strdup (flag->name) : NULL;
}
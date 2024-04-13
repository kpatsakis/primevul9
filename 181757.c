static ut64 initializeEsil(RCore *core) {
	int romem = r_config_get_i (core->config, "esil.romem");
	int stats = r_config_get_i (core->config, "esil.stats");
	int iotrap = r_config_get_i (core->config, "esil.iotrap");
	int exectrap = r_config_get_i (core->config, "esil.exectrap");
	int stacksize = r_config_get_i (core->config, "esil.stack.depth");
	int noNULL = r_config_get_i (core->config, "esil.noNULL");
	unsigned int addrsize = r_config_get_i (core->config, "esil.addr.size");
	if (!(core->anal->esil = r_anal_esil_new (stacksize, iotrap, addrsize))) {
		return UT64_MAX;
	}
	ut64 addr;
	RAnalEsil *esil = core->anal->esil;
	esil->verbose = r_config_get_i (core->config, "esil.verbose");
	esil->cmd = r_core_esil_cmd;
	r_anal_esil_setup (esil, core->anal, romem, stats, noNULL); // setup io
	{
		const char *cmd_esil_step = r_config_get (core->config, "cmd.esil.step");
		if (cmd_esil_step && *cmd_esil_step) {
			esil->cmd_step = strdup (cmd_esil_step);
		}
		const char *cmd_esil_step_out = r_config_get (core->config, "cmd.esil.stepout");
		if (cmd_esil_step_out && *cmd_esil_step_out) {
			esil->cmd_step_out = strdup (cmd_esil_step_out);
		}
		{
			const char *s = r_config_get (core->config, "cmd.esil.intr");
			if (s) {
				char *my = strdup (s);
				if (my) {
					r_config_set (core->config, "cmd.esil.intr", my);
					free (my);
				}
			}
		}
	}
	esil->exectrap = exectrap;
	RList *entries = r_bin_get_entries (core->bin);
	RBinAddr *entry = NULL;
	RBinInfo *info = NULL;
	if (entries && !r_list_empty (entries)) {
		entry = (RBinAddr *)r_list_pop (entries);
		info = r_bin_get_info (core->bin);
		addr = info->has_va? entry->vaddr: entry->paddr;
		r_list_push (entries, entry);
	} else {
		addr = core->offset;
	}
	// set memory read only
	return addr;
}
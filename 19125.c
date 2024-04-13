R_API ut64 r_core_anal_address(RCore *core, ut64 addr) {
	ut64 types = 0;
	RRegSet *rs = NULL;
	if (!core) {
		return 0;
	}
	if (core->dbg && core->dbg->reg) {
		rs = r_reg_regset_get (core->dbg->reg, R_REG_TYPE_GPR);
	}
	if (rs) {
		RRegItem *r;
		RListIter *iter;
		r_list_foreach (rs->regs, iter, r) {
			if (r->type == R_REG_TYPE_GPR) {
				ut64 val = r_reg_getv(core->dbg->reg, r->name);
				if (addr == val) {
					types |= R_ANAL_ADDR_TYPE_REG;
					break;
				}
			}
		}
	}
	if (r_flag_get_i (core->flags, addr)) {
		types |= R_ANAL_ADDR_TYPE_FLAG;
	}
	if (r_anal_get_fcn_in (core->anal, addr, 0)) {
		types |= R_ANAL_ADDR_TYPE_FUNC;
	}
	// check registers
	if (core->bin && core->dbg && r_config_get_b (core->config, "cfg.debug")) {
		RDebugMap *map;
		RListIter *iter;
		// use 'dm'
		// XXX: this line makes r2 debugging MUCH slower
		// r_debug_map_sync (core->dbg);
		r_list_foreach (core->dbg->maps, iter, map) {
			if (addr >= map->addr && addr < map->addr_end) {
				if (map->name && map->name[0] == '/') {
					if (core->io && core->io->desc &&
						core->io->desc->name &&
						!strcmp (map->name,
							 core->io->desc->name)) {
						types |= R_ANAL_ADDR_TYPE_PROGRAM;
					} else {
						types |= R_ANAL_ADDR_TYPE_LIBRARY;
					}
				}
				if (map->perm & R_PERM_X) {
					types |= R_ANAL_ADDR_TYPE_EXEC;
				}
				if (map->perm & R_PERM_R) {
					types |= R_ANAL_ADDR_TYPE_READ;
				}
				if (map->perm & R_PERM_W) {
					types |= R_ANAL_ADDR_TYPE_WRITE;
				}
				// find function
				if (map->name && strstr (map->name, "heap")) {
					types |= R_ANAL_ADDR_TYPE_HEAP;
				}
				if (map->name && strstr (map->name, "stack")) {
					types |= R_ANAL_ADDR_TYPE_STACK;
				}
				break;
			}
		}
	} else {
		int _perm = -1;
		if (core->io) {
			// sections
			RIOBank *bank = r_io_bank_get (core->io, core->io->bank);
			if (bank) {
				RIOMapRef *mapref;
				RListIter *iter;
				r_list_foreach (bank->maprefs, iter, mapref) {
					RIOMap *s = r_io_map_get (core->io, mapref->id);
					if (addr >= s->itv.addr && addr < (s->itv.addr + s->itv.size)) {
						// sections overlap, so we want to get the one with lower perms
						_perm = (_perm != -1) ? R_MIN (_perm, s->perm) : s->perm;
						// TODO: we should identify which maps come from the program or other
						//types |= R_ANAL_ADDR_TYPE_PROGRAM;
						// find function those sections should be created by hand or esil init
						if (s->name && strstr (s->name, "heap")) {
							types |= R_ANAL_ADDR_TYPE_HEAP;
						}
						if (s->name && strstr (s->name, "stack")) {
							types |= R_ANAL_ADDR_TYPE_STACK;
						}
					}
				}
			}
		}
		if (_perm != -1) {
			if (_perm & R_PERM_X) {
				types |= R_ANAL_ADDR_TYPE_EXEC;
			}
			if (_perm & R_PERM_R) {
				types |= R_ANAL_ADDR_TYPE_READ;
			}
			if (_perm & R_PERM_W) {
				types |= R_ANAL_ADDR_TYPE_WRITE;
			}
		}
	}

	// check if it's ascii
	if (addr != 0) {
		int not_ascii = 0;
		int i, failed_sequence, dir, on;
		for (i = 0; i < 8; i++) {
			ut8 n = (addr >> (i * 8)) & 0xff;
			if (n && !IS_PRINTABLE (n)) {
				not_ascii = 1;
			}
		}
		if (!not_ascii) {
			types |= R_ANAL_ADDR_TYPE_ASCII;
		}
		failed_sequence = 0;
		dir = on = -1;
		for (i = 0; i < 8; i++) {
			ut8 n = (addr >> (i * 8)) & 0xff;
			if (on != -1) {
				if (dir == -1) {
					dir = (n > on)? 1: -1;
				}
				if (n == on + dir) {
					// ok
				} else {
					failed_sequence = 1;
					break;
				}
			}
			on = n;
		}
		if (!failed_sequence) {
			types |= R_ANAL_ADDR_TYPE_SEQUENCE;
		}
	}
	return types;
}
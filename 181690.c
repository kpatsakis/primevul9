static void cmd_anal_aftertraps(RCore *core, const char *input) {
	int bufi, minop = 1; // 4
	ut8 *buf;
	RAnalOp op = {0};
	ut64 addr, addr_end;
	ut64 len = r_num_math (core->num, input);
	if (len > 0xffffff) {
		eprintf ("Too big\n");
		return;
	}
	RBinFile *bf = r_bin_cur (core->bin);
	if (!bf) {
		return;
	}
	addr = core->offset;
	if (!len) {
		// ignore search.in to avoid problems. analysis != search
		RIOMap *map = r_io_map_get (core->io, addr);
		if (map && (map->perm & R_PERM_X)) {
			// search in current section
			if (map->itv.size > bf->size) {
				addr = map->itv.addr;
				if (bf->size > map->delta) {
					len = bf->size - map->delta;
				} else {
					eprintf ("Opps something went wrong aac\n");
					return;
				}
			} else {
				addr = map->itv.addr;
				len = map->itv.size;
			}
		} else {
			if (map && map->itv.addr != map->delta && bf->size > (core->offset - map->itv.addr + map->delta)) {
				len = bf->size - (core->offset - map->itv.addr + map->delta);
			} else {
				if (bf->size > core->offset) {
					len = bf->size - core->offset;
				} else {
					eprintf ("Oops invalid range\n");
					len = 0;
				}
			}
		}
	}
	addr_end = addr + len;
	if (!(buf = malloc (4096))) {
		return;
	}
	bufi = 0;
	int trapcount = 0;
	int nopcount = 0;
	r_cons_break_push (NULL, NULL);
	while (addr < addr_end) {
		if (r_cons_is_breaked ()) {
			break;
		}
		// TODO: too many ioreads here
		if (bufi > 4000) {
			bufi = 0;
		}
		if (!bufi) {
			r_io_read_at (core->io, addr, buf, 4096);
		}
		if (r_anal_op (core->anal, &op, addr, buf + bufi, 4096 - bufi, R_ANAL_OP_MASK_BASIC)) {
			if (op.size < 1) {
				// XXX must be +4 on arm/mips/.. like we do in disasm.c
				op.size = minop;
			}
			if (op.type == R_ANAL_OP_TYPE_TRAP) {
				trapcount ++;
			} else if (op.type == R_ANAL_OP_TYPE_NOP) {
				nopcount ++;
			} else {
				if (nopcount > 1) {
					r_cons_printf ("af @ 0x%08"PFMT64x"\n", addr);
					nopcount = 0;
				}
				if (trapcount > 0) {
					r_cons_printf ("af @ 0x%08"PFMT64x"\n", addr);
					trapcount = 0;
				}
			}
		} else {
			op.size = minop;
		}
		addr += (op.size > 0)? op.size : 1;
		bufi += (op.size > 0)? op.size : 1;
		r_anal_op_fini (&op);
	}
	r_cons_break_pop ();
	free (buf);
}
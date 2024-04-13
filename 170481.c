static int parse_thread(struct MACH0_(obj_t) * bin, struct load_command *lc, ut64 off, bool is_first_thread) {
	ut64 ptr_thread, pc = UT64_MAX, pc_offset = UT64_MAX;
	ut32 flavor, count;
	ut8 *arw_ptr = NULL;
	int arw_sz, len = 0;
	ut8 thc[sizeof(struct thread_command)] = { 0 };
	ut8 tmp[4];

	if (off > bin->size || off + sizeof(struct thread_command) > bin->size) {
		return false;
	}

	len = rz_buf_read_at(bin->b, off, thc, 8);
	if (len < 1) {
		goto wrong_read;
	}
	bin->thread.cmd = rz_read_ble32(&thc[0], bin->big_endian);
	bin->thread.cmdsize = rz_read_ble32(&thc[4], bin->big_endian);
	if (rz_buf_read_at(bin->b, off + sizeof(struct thread_command), tmp, 4) < 4) {
		goto wrong_read;
	}
	flavor = rz_read_ble32(tmp, bin->big_endian);

	if (off + sizeof(struct thread_command) + sizeof(flavor) > bin->size ||
		off + sizeof(struct thread_command) + sizeof(flavor) + sizeof(ut32) > bin->size) {
		return false;
	}

	// TODO: use count for checks
	if (rz_buf_read_at(bin->b, off + sizeof(struct thread_command) + sizeof(flavor), tmp, 4) < 4) {
		goto wrong_read;
	}
	ptr_thread = off + sizeof(struct thread_command) + sizeof(flavor) + sizeof(count);

	if (ptr_thread > bin->size) {
		return false;
	}

	switch (bin->hdr.cputype) {
	case CPU_TYPE_I386:
	case CPU_TYPE_X86_64:
		switch (flavor) {
		case X86_THREAD_STATE32:
			if (ptr_thread + sizeof(struct x86_thread_state32) > bin->size) {
				return false;
			}
			if (rz_buf_fread_at(bin->b, ptr_thread,
				    (ut8 *)&bin->thread_state.x86_32, "16i", 1) == -1) {
				bprintf("Error: read (thread state x86_32)\n");
				return false;
			}
			pc = bin->thread_state.x86_32.eip;
			pc_offset = ptr_thread + rz_offsetof(struct x86_thread_state32, eip);
			arw_ptr = (ut8 *)&bin->thread_state.x86_32;
			arw_sz = sizeof(struct x86_thread_state32);
			break;
		case X86_THREAD_STATE64:
			if (ptr_thread + sizeof(struct x86_thread_state64) > bin->size) {
				return false;
			}
			if (rz_buf_fread_at(bin->b, ptr_thread,
				    (ut8 *)&bin->thread_state.x86_64, "32l", 1) == -1) {
				bprintf("Error: read (thread state x86_64)\n");
				return false;
			}
			pc = bin->thread_state.x86_64.rip;
			pc_offset = ptr_thread + rz_offsetof(struct x86_thread_state64, rip);
			arw_ptr = (ut8 *)&bin->thread_state.x86_64;
			arw_sz = sizeof(struct x86_thread_state64);
			break;
			// default: bprintf ("Unknown type\n");
		}
		break;
	case CPU_TYPE_POWERPC:
	case CPU_TYPE_POWERPC64:
		if (flavor == X86_THREAD_STATE32) {
			if (ptr_thread + sizeof(struct ppc_thread_state32) > bin->size) {
				return false;
			}
			if (rz_buf_fread_at(bin->b, ptr_thread,
				    (ut8 *)&bin->thread_state.ppc_32, bin->big_endian ? "40I" : "40i", 1) == -1) {
				bprintf("Error: read (thread state ppc_32)\n");
				return false;
			}
			pc = bin->thread_state.ppc_32.srr0;
			pc_offset = ptr_thread + rz_offsetof(struct ppc_thread_state32, srr0);
			arw_ptr = (ut8 *)&bin->thread_state.ppc_32;
			arw_sz = sizeof(struct ppc_thread_state32);
		} else if (flavor == X86_THREAD_STATE64) {
			if (ptr_thread + sizeof(struct ppc_thread_state64) > bin->size) {
				return false;
			}
			if (rz_buf_fread_at(bin->b, ptr_thread,
				    (ut8 *)&bin->thread_state.ppc_64, bin->big_endian ? "34LI3LI" : "34li3li", 1) == -1) {
				bprintf("Error: read (thread state ppc_64)\n");
				return false;
			}
			pc = bin->thread_state.ppc_64.srr0;
			pc_offset = ptr_thread + rz_offsetof(struct ppc_thread_state64, srr0);
			arw_ptr = (ut8 *)&bin->thread_state.ppc_64;
			arw_sz = sizeof(struct ppc_thread_state64);
		}
		break;
	case CPU_TYPE_ARM:
		if (ptr_thread + sizeof(struct arm_thread_state32) > bin->size) {
			return false;
		}
		if (rz_buf_fread_at(bin->b, ptr_thread,
			    (ut8 *)&bin->thread_state.arm_32, bin->big_endian ? "17I" : "17i", 1) == -1) {
			bprintf("Error: read (thread state arm)\n");
			return false;
		}
		pc = bin->thread_state.arm_32.r15;
		pc_offset = ptr_thread + rz_offsetof(struct arm_thread_state32, r15);
		arw_ptr = (ut8 *)&bin->thread_state.arm_32;
		arw_sz = sizeof(struct arm_thread_state32);
		break;
	case CPU_TYPE_ARM64:
		if (ptr_thread + sizeof(struct arm_thread_state64) > bin->size) {
			return false;
		}
		if (rz_buf_fread_at(bin->b, ptr_thread,
			    (ut8 *)&bin->thread_state.arm_64, bin->big_endian ? "34LI1I" : "34Li1i", 1) == -1) {
			bprintf("Error: read (thread state arm)\n");
			return false;
		}
		pc = rz_read_be64(&bin->thread_state.arm_64.pc);
		pc_offset = ptr_thread + rz_offsetof(struct arm_thread_state64, pc);
		arw_ptr = (ut8 *)&bin->thread_state.arm_64;
		arw_sz = sizeof(struct arm_thread_state64);
		break;
	default:
		bprintf("Error: read (unknown thread state structure)\n");
		return false;
	}

	// TODO: this shouldnt be an bprintf...
	if (arw_ptr && arw_sz > 0) {
		int i;
		ut8 *p = arw_ptr;
		bprintf("arw ");
		for (i = 0; i < arw_sz; i++) {
			bprintf("%02x", 0xff & p[i]);
		}
		bprintf("\n");
	}

	if (is_first_thread) {
		bin->main_cmd = *lc;
		if (pc != UT64_MAX) {
			bin->entry = pc;
		}
		if (pc_offset != UT64_MAX) {
			sdb_num_set(bin->kv, "mach0.entry.offset", pc_offset, 0);
		}
	}

	return true;
wrong_read:
	bprintf("Error: read (thread)\n");
	return false;
}
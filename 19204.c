static bool esilbreak_mem_write(RAnalEsil *esil, ut64 addr, const ut8 *buf, int len) {
	handle_var_stack_access (esil, addr, R_ANAL_VAR_ACCESS_TYPE_WRITE, len);
	return true;
}
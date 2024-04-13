static const char *reg_name_for_access(RAnalOp* op, RAnalVarAccessType type) {
	if (type == R_ANAL_VAR_ACCESS_TYPE_WRITE) {
		if (op->dst && op->dst->reg) {
			return op->dst->reg->name;
		}
	} else {
		if (op->src[0] && op->src[0]->reg) {
			return op->src[0]->reg->name;
		}
	}
	return NULL;
}
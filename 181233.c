bool nfsd4_cache_this_op(struct nfsd4_op *op)
{
	if (op->opnum == OP_ILLEGAL)
		return false;
	return OPDESC(op)->op_flags & OP_CACHEME;
}
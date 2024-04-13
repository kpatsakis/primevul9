const struct nfsd4_operation *OPDESC(struct nfsd4_op *op)
{
	return &nfsd4_ops[op->opnum];
}
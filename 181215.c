void warn_on_nonidempotent_op(struct nfsd4_op *op)
{
	if (OPDESC(op)->op_flags & OP_MODIFIES_SOMETHING) {
		pr_err("unable to encode reply to nonidempotent op %d (%s)\n",
			op->opnum, nfsd4_op_name(op->opnum));
		WARN_ON_ONCE(1);
	}
}
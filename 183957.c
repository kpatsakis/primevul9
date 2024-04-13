static int read_descriptor(struct x86_emulate_ctxt *ctxt,
			   struct x86_emulate_ops *ops,
			   void *ptr,
			   u16 *size, unsigned long *address, int op_bytes)
{
	int rc;

	if (op_bytes == 2)
		op_bytes = 3;
	*address = 0;
	rc = ops->read_std((unsigned long)ptr, (unsigned long *)size, 2,
			   ctxt->vcpu);
	if (rc)
		return rc;
	rc = ops->read_std((unsigned long)ptr + 2, address, op_bytes,
			   ctxt->vcpu);
	return rc;
}
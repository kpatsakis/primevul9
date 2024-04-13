static inline int writeback(struct x86_emulate_ctxt *ctxt,
			    struct x86_emulate_ops *ops)
{
	int rc;
	struct decode_cache *c = &ctxt->decode;

	switch (c->dst.type) {
	case OP_REG:
		/* The 4-byte case *is* correct:
		 * in 64-bit mode we zero-extend.
		 */
		switch (c->dst.bytes) {
		case 1:
			*(u8 *)c->dst.ptr = (u8)c->dst.val;
			break;
		case 2:
			*(u16 *)c->dst.ptr = (u16)c->dst.val;
			break;
		case 4:
			*c->dst.ptr = (u32)c->dst.val;
			break;	/* 64b: zero-ext */
		case 8:
			*c->dst.ptr = c->dst.val;
			break;
		}
		break;
	case OP_MEM:
		if (c->lock_prefix)
			rc = ops->cmpxchg_emulated(
					(unsigned long)c->dst.ptr,
					&c->dst.orig_val,
					&c->dst.val,
					c->dst.bytes,
					ctxt->vcpu);
		else
			rc = ops->write_emulated(
					(unsigned long)c->dst.ptr,
					&c->dst.val,
					c->dst.bytes,
					ctxt->vcpu);
		if (rc != 0)
			return rc;
		break;
	case OP_NONE:
		/* no writeback */
		break;
	default:
		break;
	}
	return 0;
}
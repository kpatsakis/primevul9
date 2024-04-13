static void print_overflow_msg(const char *func, const struct xdr_stream *xdr)
{
	dprintk("nfs: %s: prematurely hit end of receive buffer. "
		"Remaining buffer length is %tu words.\n",
		func, xdr->end - xdr->p);
}
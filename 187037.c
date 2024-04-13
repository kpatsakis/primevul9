static int decode_pathname(struct xdr_stream *xdr, struct nfs4_pathname *path)
{
	u32 n;
	__be32 *p;
	int status = 0;

	p = xdr_inline_decode(xdr, 4);
	if (unlikely(!p))
		goto out_overflow;
	n = be32_to_cpup(p);
	if (n == 0)
		goto root_path;
	dprintk("pathname4: ");
	path->ncomponents = 0;
	while (path->ncomponents < n) {
		struct nfs4_string *component = &path->components[path->ncomponents];
		status = decode_opaque_inline(xdr, &component->len, &component->data);
		if (unlikely(status != 0))
			goto out_eio;
		ifdebug (XDR)
			pr_cont("%s%.*s ",
				(path->ncomponents != n ? "/ " : ""),
				component->len, component->data);
		if (path->ncomponents < NFS4_PATHNAME_MAXCOMPONENTS)
			path->ncomponents++;
		else {
			dprintk("cannot parse %d components in path\n", n);
			goto out_eio;
		}
	}
out:
	return status;
root_path:
/* a root pathname is sent as a zero component4 */
	path->ncomponents = 1;
	path->components[0].len=0;
	path->components[0].data=NULL;
	dprintk("pathname4: /\n");
	goto out;
out_eio:
	dprintk(" status %d", status);
	status = -EIO;
	goto out;
out_overflow:
	print_overflow_msg(__func__, xdr);
	return -EIO;
}
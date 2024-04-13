nfsd4_layoutget_release(union nfsd4_op_u *u)
{
	kfree(u->layoutget.lg_content);
}
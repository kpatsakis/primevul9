nfsd4_secinfo_release(union nfsd4_op_u *u)
{
	if (u->secinfo.si_exp)
		exp_put(u->secinfo.si_exp);
}
nfsd4_secinfo_no_name_release(union nfsd4_op_u *u)
{
	if (u->secinfo_no_name.sin_exp)
		exp_put(u->secinfo_no_name.sin_exp);
}
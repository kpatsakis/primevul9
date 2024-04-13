nfsd4_read_release(union nfsd4_op_u *u)
{
	if (u->read.rd_filp)
		fput(u->read.rd_filp);
	trace_nfsd_read_done(u->read.rd_rqstp, u->read.rd_fhp,
			     u->read.rd_offset, u->read.rd_length);
}
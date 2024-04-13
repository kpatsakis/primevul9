static inline void nfsd4_increment_op_stats(u32 opnum)
{
	if (opnum >= FIRST_NFS4_OP && opnum <= LAST_NFS4_OP)
		nfsdstats.nfs4_opcount[opnum]++;
}
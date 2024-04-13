is_create_with_attrs(struct nfsd4_open *open)
{
	return open->op_create == NFS4_OPEN_CREATE
		&& (open->op_createmode == NFS4_CREATE_UNCHECKED
		    || open->op_createmode == NFS4_CREATE_GUARDED
		    || open->op_createmode == NFS4_CREATE_EXCLUSIVE4_1);
}
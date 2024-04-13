static void muscle_parse_acls(const sc_file_t* file, unsigned short* read_perm, unsigned short* write_perm, unsigned short* delete_perm)
{
	assert(read_perm && write_perm && delete_perm);
	*read_perm =  muscle_parse_singleAcl(sc_file_get_acl_entry(file, SC_AC_OP_READ));
	*write_perm =  muscle_parse_singleAcl(sc_file_get_acl_entry(file, SC_AC_OP_UPDATE));
	*delete_perm =  muscle_parse_singleAcl(sc_file_get_acl_entry(file, SC_AC_OP_DELETE));
}
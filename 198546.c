static void muscle_load_file_acls(sc_file_t* file, mscfs_file_t *file_data)
{
	muscle_load_single_acl(file, SC_AC_OP_READ, file_data->read);
	muscle_load_single_acl(file, SC_AC_OP_WRITE, file_data->write);
	muscle_load_single_acl(file, SC_AC_OP_UPDATE, file_data->write);
	muscle_load_single_acl(file, SC_AC_OP_DELETE, file_data->delete);
}
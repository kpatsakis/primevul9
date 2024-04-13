static void muscle_load_dir_acls(sc_file_t* file, mscfs_file_t *file_data)
{
	muscle_load_single_acl(file, SC_AC_OP_SELECT, 0);
	muscle_load_single_acl(file, SC_AC_OP_LIST_FILES, 0);
	muscle_load_single_acl(file, SC_AC_OP_LOCK, 0xFFFF);
	muscle_load_single_acl(file, SC_AC_OP_DELETE, file_data->delete);
	muscle_load_single_acl(file, SC_AC_OP_CREATE, file_data->write);
}
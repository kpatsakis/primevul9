static void muscle_load_single_acl(sc_file_t* file, int operation, unsigned short acl)
{
	int key;
	/* Everybody by default.... */
	sc_file_add_acl_entry(file, operation, SC_AC_NONE, 0);
	if(acl == 0xFFFF) {
		sc_file_add_acl_entry(file, operation, SC_AC_NEVER, 0);
		return;
	}
	for(key = 0; key < 16; key++) {
		if(acl >> key & 1) {
			sc_file_add_acl_entry(file, operation, SC_AC_CHV, key);
		}
	}
}
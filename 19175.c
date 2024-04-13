static bool __cb(RFlagItem *fi, void *user) {
	r_list_append (user, r_str_newf ("0x%08"PFMT64x, fi->offset));
	return true;
}
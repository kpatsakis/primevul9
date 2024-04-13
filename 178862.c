static const char *dccp_feat_oname(const u8 opt)
{
	switch (opt) {
	case DCCPO_CHANGE_L:  return "Change_L";
	case DCCPO_CONFIRM_L: return "Confirm_L";
	case DCCPO_CHANGE_R:  return "Change_R";
	case DCCPO_CONFIRM_R: return "Confirm_R";
	}
	return NULL;
}
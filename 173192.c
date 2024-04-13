static inline int bnx2x_get_assert_list_entry(struct bnx2x *bp,
					      enum storms storm,
					      int entry)
{
	switch (storm) {
	case XSTORM:
		return XSTORM_ASSERT_LIST_OFFSET(entry);
	case TSTORM:
		return TSTORM_ASSERT_LIST_OFFSET(entry);
	case CSTORM:
		return CSTORM_ASSERT_LIST_OFFSET(entry);
	case USTORM:
		return USTORM_ASSERT_LIST_OFFSET(entry);
	case MAX_STORMS:
	default:
		BNX2X_ERR("unknown storm\n");
	}
	return -EINVAL;
}
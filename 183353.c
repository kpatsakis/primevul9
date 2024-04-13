duplicate_PropertyValue (const PTPPropertyValue *src, PTPPropertyValue *dst, uint16_t type) {
	if (type == PTP_DTC_STR) {
		if (src->str)
			dst->str = strdup(src->str);
		else
			dst->str = NULL;
		return;
	}

	if (type & PTP_DTC_ARRAY_MASK) {
		unsigned int i;

		dst->a.count = src->a.count;
		dst->a.v = malloc (sizeof(src->a.v[0])*src->a.count);
		for (i=0;i<src->a.count;i++)
			duplicate_PropertyValue (&src->a.v[i], &dst->a.v[i], type & ~PTP_DTC_ARRAY_MASK);
		return;
	}
	switch (type & ~PTP_DTC_ARRAY_MASK) {
	case PTP_DTC_INT8:	dst->i8 = src->i8; break;
	case PTP_DTC_UINT8:	dst->u8 = src->u8; break;
	case PTP_DTC_INT16:	dst->i16 = src->i16; break;
	case PTP_DTC_UINT16:	dst->u16 = src->u16; break;
	case PTP_DTC_INT32:	dst->i32 = src->i32; break;
	case PTP_DTC_UINT32:	dst->u32 = src->u32; break;
	case PTP_DTC_UINT64:	dst->u64 = src->u64; break;
	case PTP_DTC_INT64:	dst->i64 = src->i64; break;
#if 0
	case PTP_DTC_INT128:	dst->i128 = src->i128; break;
	case PTP_DTC_UINT128:	dst->u128 = src->u128; break;
#endif
	default:		break;
	}
	return;
}
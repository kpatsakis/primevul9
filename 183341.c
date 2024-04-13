ptp_unpack_DPV (
	PTPParams *params, unsigned char* data, unsigned int *offset, unsigned int total,
	PTPPropertyValue* value, uint16_t datatype
) {
	if (*offset >= total)	/* we are at the end or over the end of the buffer */
		return 0;

	switch (datatype) {
	case PTP_DTC_INT8:
		CTVAL(value->i8,dtoh8a);
		break;
	case PTP_DTC_UINT8:
		CTVAL(value->u8,dtoh8a);
		break;
	case PTP_DTC_INT16:
		CTVAL(value->i16,dtoh16a);
		break;
	case PTP_DTC_UINT16:
		CTVAL(value->u16,dtoh16a);
		break;
	case PTP_DTC_INT32:
		CTVAL(value->i32,dtoh32a);
		break;
	case PTP_DTC_UINT32:
		CTVAL(value->u32,dtoh32a);
		break;
	case PTP_DTC_INT64:
		CTVAL(value->i64,dtoh64a);
		break;
	case PTP_DTC_UINT64:
		CTVAL(value->u64,dtoh64a);
		break;

	case PTP_DTC_UINT128:
		*offset += 16;
		/*fprintf(stderr,"unhandled unpack of uint128n");*/
		break;
	case PTP_DTC_INT128:
		*offset += 16;
		/*fprintf(stderr,"unhandled unpack of int128n");*/
		break;



	case PTP_DTC_AINT8:
		RARR(value,i8,dtoh8a);
		break;
	case PTP_DTC_AUINT8:
		RARR(value,u8,dtoh8a);
		break;
	case PTP_DTC_AUINT16:
		RARR(value,u16,dtoh16a);
		break;
	case PTP_DTC_AINT16:
		RARR(value,i16,dtoh16a);
		break;
	case PTP_DTC_AUINT32:
		RARR(value,u32,dtoh32a);
		break;
	case PTP_DTC_AINT32:
		RARR(value,i32,dtoh32a);
		break;
	case PTP_DTC_AUINT64:
		RARR(value,u64,dtoh64a);
		break;
	case PTP_DTC_AINT64:
		RARR(value,i64,dtoh64a);
		break;
	/* XXX: other int types are unimplemented */
	/* XXX: other int arrays are unimplemented also */
	case PTP_DTC_STR: {
		uint8_t len;
		/* XXX: max size */

		if (*offset >= total+1)
			return 0;

		value->str = ptp_unpack_string(params,data,*offset,total,&len);
		*offset += len*2+1;
		if (!value->str)
			return 1;
		break;
	}
	default:
		return 0;
	}
	return 1;
}
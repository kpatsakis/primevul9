ptp_pack_DPV (PTPParams *params, PTPPropertyValue* value, unsigned char** dpvptr, uint16_t datatype)
{
	unsigned char* dpv=NULL;
	uint32_t size=0;
	unsigned int i;

	switch (datatype) {
	case PTP_DTC_INT8:
		size=sizeof(int8_t);
		dpv=malloc(size);
		htod8a(dpv,value->i8);
		break;
	case PTP_DTC_UINT8:
		size=sizeof(uint8_t);
		dpv=malloc(size);
		htod8a(dpv,value->u8);
		break;
	case PTP_DTC_INT16:
		size=sizeof(int16_t);
		dpv=malloc(size);
		htod16a(dpv,value->i16);
		break;
	case PTP_DTC_UINT16:
		size=sizeof(uint16_t);
		dpv=malloc(size);
		htod16a(dpv,value->u16);
		break;
	case PTP_DTC_INT32:
		size=sizeof(int32_t);
		dpv=malloc(size);
		htod32a(dpv,value->i32);
		break;
	case PTP_DTC_UINT32:
		size=sizeof(uint32_t);
		dpv=malloc(size);
		htod32a(dpv,value->u32);
		break;
	case PTP_DTC_INT64:
		size=sizeof(int64_t);
		dpv=malloc(size);
		htod64a(dpv,value->i64);
		break;
	case PTP_DTC_UINT64:
		size=sizeof(uint64_t);
		dpv=malloc(size);
		htod64a(dpv,value->u64);
		break;
	case PTP_DTC_AUINT8:
		size=sizeof(uint32_t)+value->a.count*sizeof(uint8_t);
		dpv=malloc(size);
		htod32a(dpv,value->a.count);
		for (i=0;i<value->a.count;i++)
			htod8a(&dpv[sizeof(uint32_t)+i*sizeof(uint8_t)],value->a.v[i].u8);
		break;
	case PTP_DTC_AINT8:
		size=sizeof(uint32_t)+value->a.count*sizeof(int8_t);
		dpv=malloc(size);
		htod32a(dpv,value->a.count);
		for (i=0;i<value->a.count;i++)
			htod8a(&dpv[sizeof(uint32_t)+i*sizeof(int8_t)],value->a.v[i].i8);
		break;
	case PTP_DTC_AUINT16:
		size=sizeof(uint32_t)+value->a.count*sizeof(uint16_t);
		dpv=malloc(size);
		htod32a(dpv,value->a.count);
		for (i=0;i<value->a.count;i++)
			htod16a(&dpv[sizeof(uint32_t)+i*sizeof(uint16_t)],value->a.v[i].u16);
		break;
	case PTP_DTC_AINT16:
		size=sizeof(uint32_t)+value->a.count*sizeof(int16_t);
		dpv=malloc(size);
		htod32a(dpv,value->a.count);
		for (i=0;i<value->a.count;i++)
			htod16a(&dpv[sizeof(uint32_t)+i*sizeof(int16_t)],value->a.v[i].i16);
		break;
	case PTP_DTC_AUINT32:
		size=sizeof(uint32_t)+value->a.count*sizeof(uint32_t);
		dpv=malloc(size);
		htod32a(dpv,value->a.count);
		for (i=0;i<value->a.count;i++)
			htod32a(&dpv[sizeof(uint32_t)+i*sizeof(uint32_t)],value->a.v[i].u32);
		break;
	case PTP_DTC_AINT32:
		size=sizeof(uint32_t)+value->a.count*sizeof(int32_t);
		dpv=malloc(size);
		htod32a(dpv,value->a.count);
		for (i=0;i<value->a.count;i++)
			htod32a(&dpv[sizeof(uint32_t)+i*sizeof(int32_t)],value->a.v[i].i32);
		break;
	case PTP_DTC_AUINT64:
		size=sizeof(uint32_t)+value->a.count*sizeof(uint64_t);
		dpv=malloc(size);
		htod32a(dpv,value->a.count);
		for (i=0;i<value->a.count;i++)
			htod64a(&dpv[sizeof(uint32_t)+i*sizeof(uint64_t)],value->a.v[i].u64);
		break;
	case PTP_DTC_AINT64:
		size=sizeof(uint32_t)+value->a.count*sizeof(int64_t);
		dpv=malloc(size);
		htod32a(dpv,value->a.count);
		for (i=0;i<value->a.count;i++)
			htod64a(&dpv[sizeof(uint32_t)+i*sizeof(int64_t)],value->a.v[i].i64);
		break;
	/* XXX: other int types are unimplemented */
	case PTP_DTC_STR: {
		dpv=ptp_get_packed_stringcopy(params, value->str, &size);
		break;
	}
	}
	*dpvptr=dpv;
	return size;
}
ves_icall_type_GetTypeCodeInternal (MonoReflectionType *type)
{
	int t = type->type->type;

	MONO_ARCH_SAVE_REGS;

	if (type->type->byref)
		return TYPECODE_OBJECT;

handle_enum:
	switch (t) {
	case MONO_TYPE_VOID:
		return TYPECODE_OBJECT;
	case MONO_TYPE_BOOLEAN:
		return TYPECODE_BOOLEAN;
	case MONO_TYPE_U1:
		return TYPECODE_BYTE;
	case MONO_TYPE_I1:
		return TYPECODE_SBYTE;
	case MONO_TYPE_U2:
		return TYPECODE_UINT16;
	case MONO_TYPE_I2:
		return TYPECODE_INT16;
	case MONO_TYPE_CHAR:
		return TYPECODE_CHAR;
	case MONO_TYPE_PTR:
	case MONO_TYPE_U:
	case MONO_TYPE_I:
		return TYPECODE_OBJECT;
	case MONO_TYPE_U4:
		return TYPECODE_UINT32;
	case MONO_TYPE_I4:
		return TYPECODE_INT32;
	case MONO_TYPE_U8:
		return TYPECODE_UINT64;
	case MONO_TYPE_I8:
		return TYPECODE_INT64;
	case MONO_TYPE_R4:
		return TYPECODE_SINGLE;
	case MONO_TYPE_R8:
		return TYPECODE_DOUBLE;
	case MONO_TYPE_VALUETYPE:
		if (type->type->data.klass->enumtype) {
			t = mono_class_enum_basetype (type->type->data.klass)->type;
			goto handle_enum;
		} else {
			MonoClass *k =  type->type->data.klass;
			if (strcmp (k->name_space, "System") == 0) {
				if (strcmp (k->name, "Decimal") == 0)
					return TYPECODE_DECIMAL;
				else if (strcmp (k->name, "DateTime") == 0)
					return TYPECODE_DATETIME;
			}
		}
		return TYPECODE_OBJECT;
	case MONO_TYPE_STRING:
		return TYPECODE_STRING;
	case MONO_TYPE_SZARRAY:
	case MONO_TYPE_ARRAY:
	case MONO_TYPE_OBJECT:
	case MONO_TYPE_VAR:
	case MONO_TYPE_MVAR:
	case MONO_TYPE_TYPEDBYREF:
		return TYPECODE_OBJECT;
	case MONO_TYPE_CLASS:
		{
			MonoClass *k =  type->type->data.klass;
			if (strcmp (k->name_space, "System") == 0) {
				if (strcmp (k->name, "DBNull") == 0)
					return TYPECODE_DBNULL;
			}
		}
		return TYPECODE_OBJECT;
	case MONO_TYPE_GENERICINST:
		return TYPECODE_OBJECT;
	default:
		g_error ("type 0x%02x not handled in GetTypeCode()", t);
	}
	return 0;
}
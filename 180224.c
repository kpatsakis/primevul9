write_enum_value (char *mem, int type, guint64 value)
{
	switch (type) {
	case MONO_TYPE_U1:
	case MONO_TYPE_I1: {
		guint8 *p = (guint8*)mem;
		*p = value;
		break;
	}
	case MONO_TYPE_U2:
	case MONO_TYPE_I2: {
		guint16 *p = (void*)mem;
		*p = value;
		break;
	}
	case MONO_TYPE_U4:
	case MONO_TYPE_I4: {
		guint32 *p = (void*)mem;
		*p = value;
		break;
	}
	case MONO_TYPE_U8:
	case MONO_TYPE_I8: {
		guint64 *p = (void*)mem;
		*p = value;
		break;
	}
	default:
		g_assert_not_reached ();
	}
	return;
}
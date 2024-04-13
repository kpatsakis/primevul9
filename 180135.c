read_enum_value (char *mem, int type)
{
	switch (type) {
	case MONO_TYPE_U1:
		return *(guint8*)mem;
	case MONO_TYPE_I1:
		return *(gint8*)mem;
	case MONO_TYPE_U2:
		return *(guint16*)mem;
	case MONO_TYPE_I2:
		return *(gint16*)mem;
	case MONO_TYPE_U4:
		return *(guint32*)mem;
	case MONO_TYPE_I4:
		return *(gint32*)mem;
	case MONO_TYPE_U8:
		return *(guint64*)mem;
	case MONO_TYPE_I8:
		return *(gint64*)mem;
	default:
		g_assert_not_reached ();
	}
	return 0;
}
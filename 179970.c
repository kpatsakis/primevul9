ves_icall_System_Enum_compare_value_to (MonoObject *this, MonoObject *other)
{
	gpointer tdata = (char *)this + sizeof (MonoObject);
	gpointer odata = (char *)other + sizeof (MonoObject);
	MonoType *basetype = mono_class_enum_basetype (this->vtable->klass);
	g_assert (basetype);

#define COMPARE_ENUM_VALUES(ENUM_TYPE) do { \
		ENUM_TYPE me = *((ENUM_TYPE*)tdata); \
		ENUM_TYPE other = *((ENUM_TYPE*)odata); \
		if (me == other) \
			return 0; \
		return me > other ? 1 : -1; \
	} while (0)

#define COMPARE_ENUM_VALUES_RANGE(ENUM_TYPE) do { \
		ENUM_TYPE me = *((ENUM_TYPE*)tdata); \
		ENUM_TYPE other = *((ENUM_TYPE*)odata); \
		if (me == other) \
			return 0; \
		return me - other; \
	} while (0)

	switch (basetype->type) {
		case MONO_TYPE_U1:
			COMPARE_ENUM_VALUES (guint8);
		case MONO_TYPE_I1:
			COMPARE_ENUM_VALUES (gint8);
		case MONO_TYPE_CHAR:
		case MONO_TYPE_U2:
			COMPARE_ENUM_VALUES_RANGE (guint16);
		case MONO_TYPE_I2:
			COMPARE_ENUM_VALUES (gint16);
		case MONO_TYPE_U4:
			COMPARE_ENUM_VALUES (guint32);
		case MONO_TYPE_I4:
			COMPARE_ENUM_VALUES (gint32);
		case MONO_TYPE_U8:
			COMPARE_ENUM_VALUES (guint64);
		case MONO_TYPE_I8:
			COMPARE_ENUM_VALUES (gint64);
		default:
			g_error ("Implement type 0x%02x in get_hashcode", basetype->type);
	}
#undef COMPARE_ENUM_VALUES_RANGE
#undef COMPARE_ENUM_VALUES
	return 0;
}
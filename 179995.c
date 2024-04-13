ves_icall_System_Enum_get_hashcode (MonoObject *this)
{
	gpointer data = (char *)this + sizeof (MonoObject);
	MonoType *basetype = mono_class_enum_basetype (this->vtable->klass);
	g_assert (basetype);

	switch (basetype->type) {
		case MONO_TYPE_I1:	
			return *((gint8*)data);
		case MONO_TYPE_U1:
			return *((guint8*)data);
		case MONO_TYPE_CHAR:
		case MONO_TYPE_U2:
			return *((guint16*)data);
		
		case MONO_TYPE_I2:
			return *((gint16*)data);
		case MONO_TYPE_U4:
			return *((guint32*)data);
		case MONO_TYPE_I4:
			return *((gint32*)data);
		case MONO_TYPE_U8:
		case MONO_TYPE_I8: {
			gint64 value = *((gint64*)data);
			return (gint)(value & 0xffffffff) ^ (int)(value >> 32);
		}
		default:
			g_error ("Implement type 0x%02x in get_hashcode", basetype->type);
	}
	return 0;
}
mono_array_get_byte_length (MonoArray *array)
{
	MonoClass *klass;
	int length;
	int i;

	klass = array->obj.vtable->klass;

	if (array->bounds == NULL)
		length = array->max_length;
	else {
		length = 1;
		for (i = 0; i < klass->rank; ++ i)
			length *= array->bounds [i].length;
	}

	switch (klass->element_class->byval_arg.type) {
	case MONO_TYPE_I1:
	case MONO_TYPE_U1:
	case MONO_TYPE_BOOLEAN:
		return length;
	case MONO_TYPE_I2:
	case MONO_TYPE_U2:
	case MONO_TYPE_CHAR:
		return length << 1;
	case MONO_TYPE_I4:
	case MONO_TYPE_U4:
	case MONO_TYPE_R4:
		return length << 2;
	case MONO_TYPE_I:
	case MONO_TYPE_U:
		return length * sizeof (gpointer);
	case MONO_TYPE_I8:
	case MONO_TYPE_U8:
	case MONO_TYPE_R8:
		return length << 3;
	default:
		return -1;
	}
}
do_free_special (gpointer key, gpointer value, gpointer data)
{
	MonoClassField *field = key;
	guint32 offset = GPOINTER_TO_UINT (value);
	gint32 align;
	guint32 size;
	size = mono_type_size (field->type, &align);
	do_free_special_slot (offset, size);
}
e_binding_bind_property (gpointer source,
			 const gchar *source_property,
			 gpointer target,
			 const gchar *target_property,
			 GBindingFlags flags)
{
	return camel_binding_bind_property (source, source_property, target, target_property, flags);
}
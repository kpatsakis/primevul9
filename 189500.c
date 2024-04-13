e_binding_bind_property_full (gpointer source,
			      const gchar *source_property,
			      gpointer target,
			      const gchar *target_property,
			      GBindingFlags flags,
			      GBindingTransformFunc transform_to,
			      GBindingTransformFunc transform_from,
			      gpointer user_data,
			      GDestroyNotify notify)
{
	return camel_binding_bind_property_full (source, source_property, target, target_property, flags,
		transform_to, transform_from, user_data, notify);
}
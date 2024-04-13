e_binding_bind_property_with_closures (gpointer source,
				       const gchar *source_property,
				       gpointer target,
				       const gchar *target_property,
				       GBindingFlags flags,
				       GClosure *transform_to,
				       GClosure *transform_from)
{
	return camel_binding_bind_property_with_closures (source, source_property, target, target_property, flags,
		transform_to, transform_from);
}
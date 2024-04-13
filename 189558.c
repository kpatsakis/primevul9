e_type_traverse (GType parent_type,
                 ETypeFunc func,
                 gpointer user_data)
{
	GType *children;
	guint n_children, ii;

	g_return_if_fail (func != NULL);

	children = g_type_children (parent_type, &n_children);

	for (ii = 0; ii < n_children; ii++) {
		GType type = children[ii];

		/* Recurse over the child's children. */
		e_type_traverse (type, func, user_data);

		/* Skip abstract types. */
		if (G_TYPE_IS_ABSTRACT (type))
			continue;

		func (type, user_data);
	}

	g_free (children);
}
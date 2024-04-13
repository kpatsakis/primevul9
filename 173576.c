static void sc_html_free_tag(SC_HTMLTag *tag)
{
	if (!tag) return;

	g_free(tag->name);
	while (tag->attr != NULL) {
		SC_HTMLAttr *attr = (SC_HTMLAttr *)tag->attr->data;
		g_free(attr->name);
		g_free(attr->value);
		g_free(attr);
		tag->attr = g_list_remove(tag->attr, tag->attr->data);
	}
	g_free(tag);
}
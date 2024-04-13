xps_lookup_alternate_content(fz_xml *node)
{
	for (node = fz_xml_down(node); node; node = fz_xml_next(node))
	{
		if (!strcmp(fz_xml_tag(node), "mc:Choice") && fz_xml_att(node, "Requires"))
		{
			char list[64];
			char *next = list, *item;
			fz_strlcpy(list, fz_xml_att(node, "Requires"), sizeof(list));
			while ((item = fz_strsep(&next, " \t\r\n")) != NULL && (!*item || !strcmp(item, "xps")));
			if (!item)
				return fz_xml_down(node);
		}
		else if (!strcmp(fz_xml_tag(node), "mc:Fallback"))
			return fz_xml_down(node);
	}
	return NULL;
}
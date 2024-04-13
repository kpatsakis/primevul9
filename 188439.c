xps_deobfuscate_font_resource(xps_document *doc, xps_part *part)
{
	unsigned char buf[33];
	unsigned char key[16];
	char *p;
	int i;

	p = strrchr(part->name, '/');
	if (!p)
		p = part->name;

	for (i = 0; i < 32 && *p; p++)
	{
		if (ishex(*p))
			buf[i++] = *p;
	}
	buf[i] = 0;

	if (i != 32)
	{
		fz_warn(doc->ctx, "cannot extract GUID from obfuscated font part name");
		return;
	}

	for (i = 0; i < 16; i++)
		key[i] = unhex(buf[i*2+0]) * 16 + unhex(buf[i*2+1]);

	for (i = 0; i < 16; i++)
	{
		part->data[i] ^= key[15-i];
		part->data[i+16] ^= key[15-i];
	}
}
ves_icall_System_Text_Encoding_InternalCodePage (gint32 *int_code_page) 
{
	const char *cset;
	const char *p;
	char *c;
	char *codepage = NULL;
	int code;
	int want_name = *int_code_page;
	int i;
	
	*int_code_page = -1;
	MONO_ARCH_SAVE_REGS;

	g_get_charset (&cset);
	c = codepage = strdup (cset);
	for (c = codepage; *c; c++){
		if (isascii (*c) && isalpha (*c))
			*c = tolower (*c);
		if (*c == '-')
			*c = '_';
	}
	/* g_print ("charset: %s\n", cset); */
	
	/* handle some common aliases */
	p = encodings [0];
	code = 0;
	for (i = 0; p != 0; ){
		if ((gssize) p < 7){
			code = (gssize) p;
			p = encodings [++i];
			continue;
		}
		if (strcmp (p, codepage) == 0){
			*int_code_page = code;
			break;
		}
		p = encodings [++i];
	}
	
	if (strstr (codepage, "utf_8") != NULL)
		*int_code_page |= 0x10000000;
	free (codepage);
	
	if (want_name && *int_code_page == -1)
		return mono_string_new (mono_domain_get (), cset);
	else
		return NULL;
}
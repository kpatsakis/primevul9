static void get_type_str(xmlNodePtr node, const char* ns, const char* type, smart_str* ret)
{
	TSRMLS_FETCH();

	if (ns) {
		xmlNsPtr xmlns;
		if (SOAP_GLOBAL(soap_version) == SOAP_1_2 &&
		    strcmp(ns,SOAP_1_1_ENC_NAMESPACE) == 0) {
			ns = SOAP_1_2_ENC_NAMESPACE;
		} else if (SOAP_GLOBAL(soap_version) == SOAP_1_1 &&
		           strcmp(ns,SOAP_1_2_ENC_NAMESPACE) == 0) {
			ns = SOAP_1_1_ENC_NAMESPACE;
		}
		xmlns = encode_add_ns(node, ns);
		smart_str_appends(ret, (char*)xmlns->prefix);
		smart_str_appendc(ret, ':');
	}
	smart_str_appendl(ret, type, strlen(type));
	smart_str_0(ret);
}
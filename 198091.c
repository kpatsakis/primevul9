xmlRegExecPushString(xmlRegExecCtxtPtr exec, const xmlChar *value,
	             void *data) {
    return(xmlRegExecPushStringInternal(exec, value, data, 0));
}
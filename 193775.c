create_request_adv(const char *task, xmlNode * msg_data,
                   const char *host_to, const char *sys_to,
                   const char *sys_from, const char *uuid_from, const char *origin)
{
    char *true_from = NULL;
    xmlNode *request = NULL;
    char *reference = generateReference(task, sys_from);

    if (uuid_from != NULL) {
        true_from = generate_hash_key(sys_from, uuid_from);
    } else if (sys_from != NULL) {
        true_from = strdup(sys_from);
    } else {
        crm_err("No sys from specified");
    }

    /* host_from will get set for us if necessary by CRMd when routed */
    request = create_xml_node(NULL, __FUNCTION__);
    crm_xml_add(request, F_CRM_ORIGIN, origin);
    crm_xml_add(request, F_TYPE, T_CRM);
    crm_xml_add(request, F_CRM_VERSION, CRM_FEATURE_SET);
    crm_xml_add(request, F_CRM_MSG_TYPE, XML_ATTR_REQUEST);
    crm_xml_add(request, F_CRM_REFERENCE, reference);
    crm_xml_add(request, F_CRM_TASK, task);
    crm_xml_add(request, F_CRM_SYS_TO, sys_to);
    crm_xml_add(request, F_CRM_SYS_FROM, true_from);

    /* HOSTTO will be ignored if it is to the DC anyway. */
    if (host_to != NULL && strlen(host_to) > 0) {
        crm_xml_add(request, F_CRM_HOST_TO, host_to);
    }

    if (msg_data != NULL) {
        add_message_xml(request, F_CRM_DATA, msg_data);
    }
    free(reference);
    free(true_from);

    return request;
}
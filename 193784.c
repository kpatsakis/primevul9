create_hello_message(const char *uuid,
                     const char *client_name, const char *major_version, const char *minor_version)
{
    xmlNode *hello_node = NULL;
    xmlNode *hello = NULL;

    if (uuid == NULL || strlen(uuid) == 0
        || client_name == NULL || strlen(client_name) == 0
        || major_version == NULL || strlen(major_version) == 0
        || minor_version == NULL || strlen(minor_version) == 0) {
        crm_err("Missing fields, Hello message will not be valid.");
        return NULL;
    }

    hello_node = create_xml_node(NULL, XML_TAG_OPTIONS);
    crm_xml_add(hello_node, "major_version", major_version);
    crm_xml_add(hello_node, "minor_version", minor_version);
    crm_xml_add(hello_node, "client_name", client_name);
    crm_xml_add(hello_node, "client_uuid", uuid);

    crm_trace("creating hello message");
    hello = create_request(CRM_OP_HELLO, hello_node, NULL, NULL, client_name, uuid);
    free_xml(hello_node);

    return hello;
}
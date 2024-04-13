crm_ipcs_send_ack(crm_client_t * c, uint32_t request, const char *tag, const char *function,
                  int line)
{
    xmlNode *ack = create_xml_node(NULL, tag);

    crm_xml_add(ack, "function", function);
    crm_xml_add_int(ack, "line", line);
    crm_ipcs_send(c, request, ack, 0);
    free_xml(ack);
}
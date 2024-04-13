nwfilterDefineXML(virConnectPtr conn,
                  const char *xml)
{
    return nwfilterDefineXMLFlags(conn, xml, 0);
}
raptor_xml_writer_cdata_counted(raptor_xml_writer* xml_writer,
                                const unsigned char *s, unsigned int len)
{
  raptor_xml_writer_write_xml_declaration(xml_writer);

  XML_WRITER_FLUSH_CLOSE_BRACKET(xml_writer);
  
  raptor_xml_escape_string_any_write(s, len,
                                      '\0',
                                      XML_WRITER_XML_VERSION(xml_writer),
                                      xml_writer->iostr);

  if(xml_writer->current_element)
    xml_writer->current_element->content_cdata_seen = 1;
}
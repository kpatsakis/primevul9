raptor_xml_writer_raw(raptor_xml_writer* xml_writer,
                      const unsigned char *s)
{
  raptor_xml_writer_write_xml_declaration(xml_writer);

  XML_WRITER_FLUSH_CLOSE_BRACKET(xml_writer);
  
  raptor_iostream_string_write(s, xml_writer->iostr);

  if(xml_writer->current_element)
    xml_writer->current_element->content_cdata_seen = 1;
}
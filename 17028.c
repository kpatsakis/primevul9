raptor_xml_writer_comment_counted(raptor_xml_writer* xml_writer,
                                  const unsigned char *s, unsigned int len)
{
  XML_WRITER_FLUSH_CLOSE_BRACKET(xml_writer);
  
  raptor_xml_writer_raw_counted(xml_writer, (const unsigned char*)"<!-- ", 5);
  raptor_xml_writer_cdata_counted(xml_writer, s, len);
  raptor_xml_writer_raw_counted(xml_writer, (const unsigned char*)" -->", 4);
}
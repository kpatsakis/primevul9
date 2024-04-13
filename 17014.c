raptor_xml_writer_comment(raptor_xml_writer* xml_writer,
                          const unsigned char *s)
{
  XML_WRITER_FLUSH_CLOSE_BRACKET(xml_writer);
  
  raptor_xml_writer_raw_counted(xml_writer, (const unsigned char*)"<!-- ", 5);
  raptor_xml_writer_cdata(xml_writer, s);
  raptor_xml_writer_raw_counted(xml_writer, (const unsigned char*)" -->", 4);
}
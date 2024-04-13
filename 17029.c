raptor_xml_writer_empty_element(raptor_xml_writer* xml_writer,
                                raptor_xml_element *element)
{
  raptor_xml_writer_write_xml_declaration(xml_writer);

  XML_WRITER_FLUSH_CLOSE_BRACKET(xml_writer);
  
  if(xml_writer->pending_newline || XML_WRITER_AUTO_INDENT(xml_writer))
    raptor_xml_writer_indent(xml_writer);
  
  raptor_xml_writer_start_element_common(xml_writer, element, 1);

  raptor_xml_writer_end_element_common(xml_writer, element, 1);
  
  raptor_namespaces_end_for_depth(xml_writer->nstack, xml_writer->depth);
}
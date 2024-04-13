raptor_xml_writer_end_element(raptor_xml_writer* xml_writer,
                              raptor_xml_element* element)
{
  int is_empty;

  xml_writer->depth--;
  
  if(xml_writer->pending_newline ||
     (XML_WRITER_AUTO_INDENT(xml_writer) && element->content_element_seen))
    raptor_xml_writer_indent(xml_writer);

  is_empty = XML_WRITER_AUTO_EMPTY(xml_writer) ?
    !(element->content_cdata_seen || element->content_element_seen) : 0;
  
  raptor_xml_writer_end_element_common(xml_writer, element, is_empty);
  
  raptor_namespaces_end_for_depth(xml_writer->nstack, xml_writer->depth);

  if(xml_writer->current_element)
    xml_writer->current_element = xml_writer->current_element->parent;
}
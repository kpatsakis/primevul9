raptor_xml_writer_start_element(raptor_xml_writer* xml_writer,
                                raptor_xml_element *element)
{
  raptor_xml_writer_write_xml_declaration(xml_writer);

  XML_WRITER_FLUSH_CLOSE_BRACKET(xml_writer);
  
  if(xml_writer->pending_newline || XML_WRITER_AUTO_INDENT(xml_writer))
    raptor_xml_writer_indent(xml_writer);
  
  raptor_xml_writer_start_element_common(xml_writer, element,
                                         XML_WRITER_AUTO_EMPTY(xml_writer));

  xml_writer->depth++;

  /* SJS Note: This "if" clause is necessary because raptor_rdfxml.c
   * uses xml_writer for parseType="literal" and passes in elements
   * whose parent field is already set. The first time this function
   * is called, it sets element->parent to 0, causing the warn-07.rdf
   * test to fail. Subsequent calls to this function set
   * element->parent to its existing value. 
   */
  if(xml_writer->current_element)
    element->parent = xml_writer->current_element;
  
  xml_writer->current_element = element;
  if(element->parent)
    element->parent->content_element_seen = 1;
}
raptor_xml_writer_indent(raptor_xml_writer *xml_writer)
{
  int num_spaces;

  if(!XML_WRITER_AUTO_INDENT(xml_writer)) {
    if(xml_writer->pending_newline) {
      raptor_iostream_write_byte('\n', xml_writer->iostr);
      xml_writer->pending_newline = 0;

      if(xml_writer->current_element)
        xml_writer->current_element->content_cdata_seen = 1;
    }
    return 0;
  }
  
  num_spaces = xml_writer->depth * XML_WRITER_INDENT(xml_writer);

  /* Do not write an extra newline at the start of the document
   * (after the XML declaration or XMP processing instruction has
   * been writtten)
   */
  if(xml_writer->xml_declaration_checked == 1)
    xml_writer->xml_declaration_checked++;
  else {
    raptor_iostream_write_byte('\n', xml_writer->iostr);
    xml_writer->pending_newline = 0;
  }
  
  while(num_spaces > 0) {

    int count = (num_spaces > RAPTOR_GOOD_CAST(int, SPACES_BUFFER_SIZE)) ?
                 RAPTOR_GOOD_CAST(int, SPACES_BUFFER_SIZE) : num_spaces;

    raptor_iostream_counted_string_write(spaces_buffer, count,
                                         xml_writer->iostr);

    num_spaces -= count;
  }

  if(xml_writer->current_element)
    xml_writer->current_element->content_cdata_seen = 1;

  return 0;
}
raptor_xml_writer_end_element_common(raptor_xml_writer* xml_writer,
                                     raptor_xml_element *element,
                                     int is_empty)
{
  raptor_iostream* iostr = xml_writer->iostr;

  if(is_empty)
    raptor_iostream_write_byte('/', iostr);
  else {
    
    raptor_iostream_write_byte('<', iostr);

    raptor_iostream_write_byte('/', iostr);

    if(element->name->nspace && element->name->nspace->prefix_length > 0) {
      raptor_iostream_counted_string_write((const char*)element->name->nspace->prefix, 
                                           element->name->nspace->prefix_length,
                                           iostr);
      raptor_iostream_write_byte(':', iostr);
    }
    raptor_iostream_counted_string_write((const char*)element->name->local_name,
                                         element->name->local_name_length,
                                         iostr);
  }
  
  raptor_iostream_write_byte('>', iostr);

  return 0;
  
}
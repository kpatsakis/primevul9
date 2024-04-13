raptor_xml_writer_set_option(raptor_xml_writer *xml_writer, 
                             raptor_option option, char* string, int integer)
{
  return raptor_object_options_set_option(&xml_writer->options, option,
                                          string, integer);
}
raptor_xml_writer_get_option(raptor_xml_writer *xml_writer,
                             raptor_option option,
                             char** string_p, int* integer_p)
{
  return raptor_object_options_get_option(&xml_writer->options, option,
                                          string_p, integer_p);
}
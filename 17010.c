raptor_free_xml_writer(raptor_xml_writer* xml_writer)
{
  if(!xml_writer)
    return;

  if(xml_writer->nstack && xml_writer->my_nstack)
    raptor_free_namespaces(xml_writer->nstack);

  raptor_object_options_clear(&xml_writer->options);
  
  RAPTOR_FREE(raptor_xml_writer, xml_writer);
}
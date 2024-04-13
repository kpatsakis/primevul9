raptor_new_xml_writer(raptor_world* world,
                      raptor_namespace_stack *nstack,
                      raptor_iostream* iostr)
{
  raptor_xml_writer* xml_writer;

  RAPTOR_CHECK_CONSTRUCTOR_WORLD(world);

  if(!iostr)
    return NULL;
  
  raptor_world_open(world);

  xml_writer = RAPTOR_CALLOC(raptor_xml_writer*, 1, sizeof(*xml_writer));
  if(!xml_writer)
    return NULL;

  xml_writer->world = world;

  xml_writer->nstack_depth = 0;

  xml_writer->nstack = nstack;
  if(!xml_writer->nstack) {
    xml_writer->nstack = raptor_new_namespaces(world, 1);
    xml_writer->my_nstack = 1;
  }

  xml_writer->iostr = iostr;

  raptor_object_options_init(&xml_writer->options,
                             RAPTOR_OPTION_AREA_XML_WRITER);
  
  return xml_writer;
}
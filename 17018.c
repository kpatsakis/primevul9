main(int argc, char *argv[]) 
{
  raptor_world *world;
  const char *program = raptor_basename(argv[0]);
  raptor_iostream *iostr;
  raptor_namespace_stack *nstack;
  raptor_namespace* foo_ns;
  raptor_xml_writer* xml_writer;
  raptor_uri* base_uri;
  raptor_qname* el_name;
  raptor_xml_element *element;
  unsigned long offset;
  raptor_qname **attrs;
  raptor_uri* base_uri_copy = NULL;

  /* for raptor_new_iostream_to_string */
  void *string = NULL;
  size_t string_len = 0;

  world = raptor_new_world();
  if(!world || raptor_world_open(world))
    exit(1);
  
  iostr = raptor_new_iostream_to_string(world, &string, &string_len, NULL);
  if(!iostr) {
    fprintf(stderr, "%s: Failed to create iostream to string\n", program);
    exit(1);
  }

  nstack = raptor_new_namespaces(world, 1);

  xml_writer = raptor_new_xml_writer(world, nstack, iostr);
  if(!xml_writer) {
    fprintf(stderr, "%s: Failed to create xml_writer to iostream\n", program);
    exit(1);
  }

  base_uri = raptor_new_uri(world, base_uri_string);

  foo_ns = raptor_new_namespace(nstack,
                              (const unsigned char*)"foo",
                              (const unsigned char*)"http://example.org/foo-ns#",
                              0);


  el_name = raptor_new_qname_from_namespace_local_name(world,
                                                       foo_ns,
                                                       (const unsigned char*)"bar", 
                                                       NULL);
  base_uri_copy = base_uri ? raptor_uri_copy(base_uri) : NULL;
  element = raptor_new_xml_element(el_name,
                                  NULL, /* language */
                                  base_uri_copy);

  raptor_xml_writer_start_element(xml_writer, element);
  raptor_xml_writer_cdata_counted(xml_writer, (const unsigned char*)"hello\n", 6);
  raptor_xml_writer_comment_counted(xml_writer, (const unsigned char*)"comment", 7);
  raptor_xml_writer_cdata(xml_writer, (const unsigned char*)"\n");
  raptor_xml_writer_end_element(xml_writer, element);

  raptor_free_xml_element(element);

  raptor_xml_writer_cdata(xml_writer, (const unsigned char*)"\n");

  el_name = raptor_new_qname(nstack, 
                             (const unsigned char*)"blah", 
                             NULL /* no attribute value - element */);
  base_uri_copy = base_uri ? raptor_uri_copy(base_uri) : NULL;
  element = raptor_new_xml_element(el_name,
                                   NULL, /* language */
                                   base_uri_copy);

  attrs = RAPTOR_CALLOC(raptor_qname**, 1, sizeof(raptor_qname*));
  attrs[0] = raptor_new_qname(nstack, 
                              (const unsigned char*)"a",
                              (const unsigned char*)"b" /* attribute value */);
  raptor_xml_element_set_attributes(element, attrs, 1);

  raptor_xml_writer_empty_element(xml_writer, element);

  raptor_xml_writer_cdata(xml_writer, (const unsigned char*)"\n");

  raptor_free_xml_writer(xml_writer);

  raptor_free_xml_element(element);

  raptor_free_namespace(foo_ns);

  raptor_free_namespaces(nstack);

  raptor_free_uri(base_uri);

  
  offset = raptor_iostream_tell(iostr);

#if defined(RAPTOR_DEBUG) && RAPTOR_DEBUG > 1
  fprintf(stderr, "%s: Freeing iostream\n", program);
#endif
  raptor_free_iostream(iostr);

  if(offset != OUT_BYTES_COUNT) {
    fprintf(stderr, "%s: I/O stream wrote %d bytes, expected %d\n", program,
            (int)offset, (int)OUT_BYTES_COUNT);
    fputs("[[", stderr);
    (void)fwrite(string, 1, string_len, stderr);
    fputs("]]\n", stderr);
    return 1;
  }
  
  if(!string) {
    fprintf(stderr, "%s: I/O stream failed to create a string\n", program);
    return 1;
  }
  string_len = strlen((const char*)string);
  if(string_len != offset) {
    fprintf(stderr, "%s: I/O stream created a string length %d, expected %d\n", program, (int)string_len, (int)offset);
    return 1;
  }

#if defined(RAPTOR_DEBUG) && RAPTOR_DEBUG > 1
  fprintf(stderr, "%s: Made XML string of %d bytes\n", program, (int)string_len);
  fputs("[[", stderr);
  (void)fwrite(string, 1, string_len, stderr);
  fputs("]]\n", stderr);
#endif

  raptor_free_memory(string);
  
  raptor_free_world(world);
  
  /* keep gcc -Wall happy */
  return(0);
}
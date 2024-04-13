raptor_xml_writer_nsd_compare(const void *a, const void *b)
{
  struct nsd* nsd_a = (struct nsd*)a;
  struct nsd* nsd_b = (struct nsd*)b;
  return strcmp((const char*)nsd_a->declaration, (const char*)nsd_b->declaration);
}
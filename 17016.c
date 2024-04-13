raptor_xml_writer_flush(raptor_xml_writer* xml_writer)
{
  if(xml_writer->pending_newline) {
    raptor_iostream_write_byte('\n', xml_writer->iostr);
    xml_writer->pending_newline = 0;
  }
}
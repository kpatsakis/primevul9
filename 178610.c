bool Curl_pipeline_wanted(const struct Curl_multi *multi, int bits)
{
  return (multi && (multi->pipelining & bits)) ? TRUE : FALSE;
}
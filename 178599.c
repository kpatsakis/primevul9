void Curl_multi_handlePipeBreak(struct Curl_easy *data)
{
  data->easy_conn = NULL;
}
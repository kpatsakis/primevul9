void Curl_free_request_state(struct Curl_easy *data)
{
  Curl_safefree(data->req.protop);
  Curl_safefree(data->req.newurl);
}
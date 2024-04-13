void Curl_multi_connchanged(struct Curl_multi *multi)
{
  multi->recheckstate = TRUE;
}
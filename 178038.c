static struct Curl_easy* gethandleathead(struct curl_llist *pipeline)
{
  struct curl_llist_element *curr = pipeline->head;
  if(curr) {
    return (struct Curl_easy *) curr->ptr;
  }

  return NULL;
}
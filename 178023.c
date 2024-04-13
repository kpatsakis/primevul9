static void Curl_printPipeline(struct curl_llist *pipeline)
{
  struct curl_llist_element *curr;

  curr = pipeline->head;
  while(curr) {
    struct Curl_easy *data = (struct Curl_easy *) curr->ptr;
    infof(data, "Handle in pipeline: %s\n", data->state.path);
    curr = curr->next;
  }
}
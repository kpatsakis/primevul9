static void cleanup_special_delivery(deliver_data_t *mydata)
{
    fclose(mydata->m->f);
    prot_free(mydata->m->data);
    append_removestage(mydata->stage);
    if (mydata->content->base) {
        map_free(&mydata->content->base, &mydata->content->len);
        if (mydata->content->body) {
            message_free_body(mydata->content->body);
            free(mydata->content->body);
        }
    }
}
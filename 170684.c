static void on_client_closed(gpointer user_data)
{
    ClientIO* client_io = user_data;
    GHashTableIter it;
    char* md5;
    Cache* cache;
    GSList *l;
    GIOChannel *ch = client_io->channel;

    DEBUG("client closed: %p", ch);
    g_hash_table_iter_init (&it, hash);
    while( g_hash_table_iter_next (&it, (gpointer*)&md5, (gpointer*)&cache) )
    {
        while((l = g_slist_find( cache->clients, client_io )) != NULL)
        {
            /* FIXME: some clients are closed accidentally without
             * unregister the menu first due to crashes.
             * We need to do unref for them to prevent memory leaks.
             *
             * The behavior is not currently well-defined.
             * if a client do unregister first, and then was closed,
             * unref will be called twice and incorrect ref. counting
             * will happen.
             */
            cache->clients = g_slist_delete_link( cache->clients, l );
            DEBUG("remove channel from cache %p", cache);
            if(cache->clients == NULL)
                cache_free(cache);
        }
    }
    /* DEBUG("client closed"); */

    g_source_remove(client_io->source_id);
    g_free(client_io);
}
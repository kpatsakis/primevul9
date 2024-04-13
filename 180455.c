unclose_path(gx_path * ppath, int count)
{
    subpath *psub;

    for (psub = ppath->first_subpath; count != 0;
         psub = (subpath *) psub->last->next
        )
        if (psub->last == (segment *) & psub->closer) {
            segment *prev = psub->closer.prev, *next = psub->closer.next;

            prev->next = next;
            if (next)
                next->prev = prev;
            psub->last = prev;
            count--;
        }
}
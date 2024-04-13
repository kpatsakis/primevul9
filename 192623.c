S_unwind_scan_frames(pTHX_ const void *p)
{
    scan_frame *f= (scan_frame *)p;
    do {
        scan_frame *n= f->next_frame;
        Safefree(f);
        f= n;
    } while (f);
}
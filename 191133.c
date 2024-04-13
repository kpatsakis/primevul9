gs_main_dump_stack(gs_main_instance *minst, int code, ref * perror_object)
{
    i_ctx_t *i_ctx_p = minst->i_ctx_p;

    zflush(i_ctx_p);            /* force out buffered output */
    dmprintf1(minst->heap, "\nUnexpected interpreter error %d.\n", code);
    if (perror_object != 0) {
        dmputs(minst->heap, "Error object: ");
        debug_print_ref(minst->heap, perror_object);
        dmputc(minst->heap, '\n');
    }
    debug_dump_stack(minst->heap, &o_stack, "Operand stack");
    debug_dump_stack(minst->heap, &e_stack, "Execution stack");
    debug_dump_stack(minst->heap, &d_stack, "Dictionary stack");
}
hook_process_add_to_buffer (struct t_hook *hook_process, int index_buffer,
                            const char *buffer, int size)
{
    if (HOOK_PROCESS(hook_process, buffer_size[index_buffer]) + size > HOOK_PROCESS_BUFFER_SIZE)
        hook_process_send_buffers (hook_process, WEECHAT_HOOK_PROCESS_RUNNING);

    memcpy (HOOK_PROCESS(hook_process, buffer[index_buffer]) +
            HOOK_PROCESS(hook_process, buffer_size[index_buffer]),
            buffer, size);
    HOOK_PROCESS(hook_process, buffer_size[index_buffer]) += size;
}
void cpu_exec_init_all(void)
{
    qemu_mutex_init(&ram_list.mutex);
    memory_map_init();
    io_mem_init();
    qemu_mutex_init(&map_client_list_lock);
}
size_t v9fs_readdir_response_size(V9fsString *name)
{
    /*
     * Size of each dirent on the wire: size of qid (13) + size of offset (8)
     * size of type (1) + size of name.size (2) + strlen(name.data)
     */
    return 24 + v9fs_string_size(name);
}
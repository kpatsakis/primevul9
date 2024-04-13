push_caller(const char *path, VALUE val, void *enc)
{
    struct push_glob_args *arg = (struct push_glob_args *)val;
    struct glob_pattern *list;
    int status;

    list = glob_make_pattern(path, path + strlen(path), arg->flags, enc);
    if (!list) {
	return -1;
    }
    status = glob_helper(arg->fd, arg->path, arg->baselen, arg->namelen, arg->dirsep,
			 arg->pathtype, &list, &list + 1, arg->flags, arg->funcs,
			 arg->arg, enc);
    glob_free_pattern(list);
    return status;
}
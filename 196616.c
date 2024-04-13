push_glob0_caller(const char *path, VALUE val, void *enc)
{
    struct push_glob0_args *arg = (struct push_glob0_args *)val;
    return ruby_glob0(path, arg->fd, arg->base, arg->flags, arg->funcs, arg->arg, enc);
}
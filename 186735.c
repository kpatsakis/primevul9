static int krb5_cleanup(void *ptr)
{
    struct krb5child_req *kr = talloc_get_type(ptr, struct krb5child_req);

    if (kr == NULL) return EOK;

    memset(kr, 0, sizeof(struct krb5child_req));

    return EOK;
}
static inline bool gic_has_groups(GICState *s)
{
    return s->revision == 2 || s->security_extn;
}
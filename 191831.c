real_uptime(const routerinfo_t *router, time_t now)
{
  if (now < router->cache_info.published_on)
    return router->uptime;
  else
    return router->uptime + (now - router->cache_info.published_on);
}
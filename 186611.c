_dbus_atomic_get (DBusAtomic *atomic)
{
#if DBUS_USE_SYNC
  __sync_synchronize ();
  return atomic->value;
#else
  dbus_int32_t res;

  pthread_mutex_lock (&atomic_mutex);
  res = atomic->value;
  pthread_mutex_unlock (&atomic_mutex);

  return res;
#endif
}
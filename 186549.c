_dbus_atomic_set_zero (DBusAtomic *atomic)
{
#if DBUS_USE_SYNC
  /* Atomic version of "*atomic &= 0; return *atomic" */
  __sync_and_and_fetch (&atomic->value, 0);
#else
  pthread_mutex_lock (&atomic_mutex);
  atomic->value = 0;
  pthread_mutex_unlock (&atomic_mutex);
#endif
}
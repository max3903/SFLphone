#ifndef __VIDEO_EVENT__
#define __VIDEO_EVENT__

typedef struct {
    int fd;
} sflphone_event_listener_t;

typedef enum {
    NEW_FRAME = 1
} sflphone_event_t;

/**
 * Initialize an event listener.
 * The abstract namespace representing a running instance of a "file descriptor passer" in the UNIX domain is obtained through DBUS.
 * This one will be used only once to transfer the file descriptor for the eventfd() channel.
 * @return A new instance of a sflphone_event_listener_t object. It's up to the user to free the object once finished.
 * NULL is returned in case of an error.
 * @param shm The shared memory segment allocated by the server for which to receive notifications for.
 */
sflphone_event_listener_t* sflphone_eventfd_init (const char* shm);

/**
 * Blocking call for capturing new event.
 * @param listener The event listener on which to catch new events.
 * @param The event that was captured.
 */
sflphone_event_t sflphone_eventfd_catch (sflphone_event_listener_t* listener);

/**
 * Free an sflphone_event_listener_t type of object.
 */
void sflphone_eventfd_free (sflphone_event_listener_t* listener);

#endif

#include "video_event.h"
#include "sflphone_const.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/eventfd.h>


/**
 * @param namespace The UNIX abstract namespace on which to receive the even file descriptor
 * @return The event file descriptor, -1 on error.
 */
static int
receive_eventfd (const char* namespace)
{
  // +1 being for the null character in front
  char * sun_path = (char *) malloc (strlen (namespace) + 1);
  sprintf (sun_path, "%c%s", '\0', namespace);

  struct sockaddr_un server_address;
  server_address.sun_family = AF_UNIX;
  strcpy(server_address.sun_path, sun_path);

  int len = sizeof(server_address.sun_family) + strlen (namespace) + 1;

  int client_socket = socket (PF_UNIX, SOCK_STREAM, 0);
  if (connect (client_socket, (struct sockaddr *) &server_address, len) < 0) {
    ERROR("While trying to receive event FD from %s : (%s)", namespace, strerror(errno));
    free(sun_path);
    return -1;
  }

  int file_descriptors[1];
  char buffer[CMSG_SPACE (sizeof file_descriptors)];

  char ping;
  struct iovec ping_vec =
    { .iov_base = &ping, .iov_len = sizeof ping, };

  struct msghdr message =
    { .msg_control = buffer, .msg_controllen = sizeof buffer,
        .msg_iov = &ping_vec, .msg_iovlen = 1, };

  if (recvmsg (client_socket, &message, 0) < 0) {
    ERROR("While trying to receive event FD from %s : (%s)", namespace, strerror(errno));
    free(sun_path);
    return -1;
  }

  close (client_socket);

  // Parsing
  struct cmsghdr *cmessage = CMSG_FIRSTHDR (&message);
  memcpy (file_descriptors, CMSG_DATA (cmessage), sizeof file_descriptors);

  free (sun_path);

  return file_descriptors[0];
}

sflphone_event_listener_t * sflphone_eventfd_init(const char* device)
{
  char* fd_passer = dbus_video_get_fd_passer_namespace(device);
  DEBUG("********************** Got namespace %s", fd_passer);
  if (fd_passer == NULL) {
    return NULL;
  }

  sflphone_event_listener_t * listener = (sflphone_event_listener_t*) malloc(sizeof(sflphone_event_listener_t));
  if (listener == NULL) {
    ERROR("An error occured while receiving the FD %s:%d", __FILE__, __LINE__);
    return NULL;
  }

  listener->fd = receive_eventfd(fd_passer);
  if (listener->fd < 0) {
    ERROR("An error occured while receiving the FD %s:%d", __FILE__, __LINE__);
    sflphone_eventfd_free(listener);
    return NULL;
  }

  return listener;
}

int sflphone_eventfd_free(sflphone_event_listener_t* listener)
{
  free(listener);
}

sflphone_event_t sflphone_eventfd_catch(sflphone_event_listener_t* listener)
{
  eventfd_t event;
  eventfd_read(listener->fd, &event);

  return (sflphone_event_t) event;
}

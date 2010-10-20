#include "video_event.h"
#include "sflphone_const.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stddef.h>

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
    // Connect
    struct sockaddr_un server_address;
    server_address.sun_family = AF_UNIX;
    // The '\0' character denotes an abstract namespace
    server_address.sun_path[0] = '\0';

    char* socket_path = strdup (namespace);

    if (strlen (namespace) >= 108) {
        socket_path[106] = '\0';
    }

    strcpy (server_address.sun_path + 1, socket_path);

    int client_socket = socket (PF_UNIX, SOCK_STREAM, 0);

    if (connect (client_socket, (struct sockaddr *) &server_address,
                 sizeof (server_address.sun_family) + strlen (socket_path) + 1) < 0) {
        ERROR ("VideoEndpoint: Failed to connect to the file descriptor passer on \"%s\" because \"%s\"", socket_path, strerror (errno));
        free (socket_path);
        return -1;
    }

    free (socket_path);

    // Receive the message and extract the fd
    struct msghdr msg;
    struct cmsghdr *cmsg;
    union {
        struct cmsghdr hdr;
        unsigned char buf[CMSG_SPACE (sizeof (int)) ];
    } cmsgbuf;

    memset (&msg, 0, sizeof (msg));
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_control = &cmsgbuf.buf;
    msg.msg_controllen = sizeof (cmsgbuf.buf);

    struct iovec nothing_ptr;
    char nothing;
    nothing_ptr.iov_base = &nothing;
    nothing_ptr.iov_len = 1;
    msg.msg_iov = &nothing_ptr;
    msg.msg_iovlen = 1;

    if (recvmsg (client_socket, &msg, 0) == -1) {
        ERROR ("VideoEndpoint: recvmsg() : %s:%d %s", __FILE__, __LINE__, strerror (errno));
    }

    if ( (msg.msg_flags & MSG_TRUNC) || (msg.msg_flags & MSG_CTRUNC)) {
        ERROR ("VideoEndpoint: Control message truncated : %s:%d", __FILE__, __LINE__);
    }

    int fd;

    for (cmsg = CMSG_FIRSTHDR (&msg); cmsg != NULL; cmsg = CMSG_NXTHDR (&msg, cmsg)) {
        if (cmsg->cmsg_len == CMSG_LEN (sizeof (int)) && cmsg->cmsg_level
                == SOL_SOCKET && cmsg->cmsg_type == SCM_RIGHTS) {
            fd = * (int *) CMSG_DATA (cmsg);
            // Return the first fd for the moment
            return fd;
        }
    }

    return -1;
}

sflphone_event_listener_t *
sflphone_eventfd_init (const char* shm)
{
    char* fd_passer = dbus_video_get_fd_passer_namespace (shm);

    if (fd_passer == NULL) {
        ERROR ("VideoEndpoint: Could not retreive namespace from dbus (%s:%d)", __FILE__, __LINE__);
        return NULL;
    }

    sflphone_event_listener_t * listener = (sflphone_event_listener_t*) malloc (
                                               sizeof (sflphone_event_listener_t));

    if (listener == NULL) {
        ERROR ("VideoEndpoint: An error occured while receiving the FD %s:%d", __FILE__, __LINE__);
        return NULL;
    }

    listener->fd = receive_eventfd (fd_passer);

    if (listener->fd < 0) {
        ERROR ("VideoEndpoint: An error occured while receiving the FD %s:%d", __FILE__, __LINE__);
        sflphone_eventfd_free (listener);
        return NULL;
    }

    return listener;
}

void
sflphone_eventfd_free (sflphone_event_listener_t* listener)
{
    free (listener);
}

sflphone_event_t
sflphone_eventfd_catch (sflphone_event_listener_t* listener)
{
    eventfd_t event;
    eventfd_read (listener->fd, &event);

    return (sflphone_event_t) event;
}

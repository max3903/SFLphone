#include "FileDescriptorPasser.h"
#include "logger.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/eventfd.h>

#define ANCIL_FD_BUFFER(n) \
    struct { \
	struct cmsghdr h; \
	int fd[n]; \
    }

namespace sfl {
FileDescriptorPasser::FileDescriptorPasser(
		const std::string& abstractNamespace, int fd) :
	serverSocket(0), fdPassed(fd), path(abstractNamespace), ready(false) {
}

void FileDescriptorPasser::initial() {
	_debug("Initializing thread %s", __FILE__);

	serverSocket = socket(PF_UNIX, SOCK_STREAM, 0);

	struct sockaddr_un server_address = { AF_UNIX, "\0org.sflphone.eventfd" };

	if (bind(serverSocket, (struct sockaddr *) &server_address,
			sizeof server_address) < 0) {
		_error("Failed to bind() %s:%d: %s", __FILE__, __LINE__, strerror(errno));
	}

	if (listen(serverSocket, 1) < 0) {
		_error("Failed to listen() %s:%d: %s", __FILE__, __LINE__, strerror(errno));
	}

	ready = true;
}

int FileDescriptorPasser::sendFd(int clientfd) {
	struct msghdr msg;
	struct cmsghdr *cmsg;
	union {
		struct cmsghdr hdr;
		unsigned char buf[CMSG_SPACE(sizeof(int))];
	} cmsgbuf;

	memset(&msg, 0, sizeof(msg));
	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_control = &cmsgbuf.buf;
	msg.msg_controllen = sizeof(cmsgbuf.buf);

	struct iovec nothing_ptr;
	char nothing;
	nothing_ptr.iov_base = &nothing;
	nothing_ptr.iov_len = 1;
	msg.msg_iov = &nothing_ptr;
	msg.msg_iovlen = 1;

	cmsg = CMSG_FIRSTHDR(&msg);
	cmsg->cmsg_len = CMSG_LEN(sizeof(int));
	cmsg->cmsg_level = SOL_SOCKET;
	cmsg->cmsg_type = SCM_RIGHTS;
	*(int *) CMSG_DATA(cmsg) = fdPassed;

	return sendmsg(clientfd, &msg, 0);
}

void FileDescriptorPasser::run() {

	_debug("Starting FD passer %s", path.c_str());

	while (isRunning()) {
		struct sockaddr_un clientAddress;
		socklen_t clientAddressLength = sizeof clientAddress;

		_debug("Accepting ...");
		int clientConnection = accept(serverSocket,
				(struct sockaddr *) &clientAddress, &clientAddressLength);
		_debug("Client accepted.");

		if (sendFd(clientConnection) < 0) {
			_error("Failed to sendFd() %s:%d: %s", __FILE__, __LINE__, strerror(errno));
		}

		::close(clientConnection);

		yield();
	}

	_debug ("FD passer thread exited.");
}

void FileDescriptorPasser::final() {
	::close(serverSocket);
	ready = false;
}

}

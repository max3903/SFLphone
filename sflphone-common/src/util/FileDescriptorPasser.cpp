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

namespace sfl {
FileDescriptorPasser::FileDescriptorPasser(
		const std::string& abstractNamespace, int fd) :
	serverSocket(0), passedFd(fd), path(abstractNamespace) {
	initMessage();
}

void FileDescriptorPasser::initMessage() {
	fileDescriptors[0] = passedFd;

	message.msg_control = messageControlBuffer;
	message.msg_controllen = sizeof messageControlBuffer;

	cmessage = CMSG_FIRSTHDR(&message);
	cmessage->cmsg_level = SOL_SOCKET;
	cmessage->cmsg_type = SCM_RIGHTS;
	cmessage->cmsg_len = CMSG_LEN(sizeof fileDescriptors);
	message.msg_controllen = cmessage->cmsg_len;

	char ping = 23;
	pingVec.iov_base = &ping;
	pingVec.iov_len = sizeof ping;

	message.msg_iov = &pingVec;
	message.msg_iovlen = 1;

	memcpy(CMSG_DATA(cmessage), fileDescriptors, sizeof fileDescriptors);
}

void FileDescriptorPasser::initial() {
	serverSocket = socket(PF_UNIX, SOCK_STREAM, 0);

	char * sun_path = (char *) malloc(strlen(path.c_str()) + 1);
	sprintf(sun_path, "%c%s", '\0', path.c_str());

	struct sockaddr_un server_address;
	server_address.sun_family = AF_UNIX;
	strcpy(server_address.sun_path, sun_path);

	int len = sizeof(server_address.sun_family) + strlen(
			path.c_str()) + 1;

	if (bind(serverSocket, (struct sockaddr *) &server_address, len) < 0) {
		_error("Failed to bind() %s:%d: %s", __FILE__, __LINE__, strerror(errno));
	}

	if (listen(serverSocket, 1) < 0) {
		_error("Failed to listen() %s:%d: %s", __FILE__, __LINE__, strerror(errno));
	}
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

		if (sendmsg(clientConnection, &message, 0) < 0) {
			_error("Failed to sendmsg() %s:%d: %s", __FILE__, __LINE__, strerror(errno));
		}

		::close(clientConnection);

		yield();
	}

	_debug ("FD passer thread exited.");
}

void FileDescriptorPasser::final() {
	::close(serverSocket);
}

}

/*
 * ident_tool.c
 * Author: Landon Fuller <landonf@fullers.org>
 * This software is public domain
 * Code based on examples from R. Stevens
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netdb.h>

ssize_t write_fd(int fd, void *ptr, size_t nbytes, int sendfd) {
	struct msghdr msg;
	struct iovec iov[1];

	union {
		struct cmsghdr cm;
		char control[CMSG_SPACE(sizeof(int))];
	} control_un;
	struct cmsghdr *cmptr;
	
	msg.msg_control = control_un.control;
	msg.msg_controllen = sizeof(control_un.control);

	cmptr = CMSG_FIRSTHDR(&msg);
	cmptr->cmsg_len = CMSG_LEN(sizeof(int));
	cmptr->cmsg_level = SOL_SOCKET;
	cmptr->cmsg_type = SCM_RIGHTS;
	*((int *) CMSG_DATA(cmptr)) = sendfd;

	msg.msg_name = NULL;
	msg.msg_namelen = 0;

	iov[0].iov_base = ptr;
	iov[0].iov_len = nbytes;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;

	return (sendmsg(fd, &msg, 0));
}

int main(int argc, char **argv) {
	int n, sockfd;
	struct addrinfo hints, *res;
	
	memset(&hints, 0, sizeof(struct addrinfo));
	
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = PF_INET6;
	hints.ai_socktype = SOCK_STREAM;
	
	if ((n = getaddrinfo(NULL, "ident", &hints, &res)) == -1)
		return 1;
	
	if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1)
		return 1;
	
	if ((bind(sockfd, res->ai_addr, res->ai_addrlen)) < 0)
		return 1;
        
	if (write_fd(STDOUT_FILENO, "", 1, sockfd) < 0)
		return 1;
	
	freeaddrinfo(res);
	
	return 0;
}

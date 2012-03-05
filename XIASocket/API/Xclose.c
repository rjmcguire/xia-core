/* ts=4 */
/*
** Copyright 2011 Carnegie Mellon University
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**    http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/
/*!
** @file Xclose.c
** @brief implements Xclose
*/

#include "Xsocket.h"
#include "Xinit.h"
#include "Xutil.h"
#include <errno.h>

/*!
** @brief Close an Xsocket.
**
** Causes click to tear down the underlying XIA socket and also closes the
** UDP socket used to talk to click.
**
** @param sockfd	The control socket
**
** @returns 0 on success
** @returns -1 on error with errno set
*/
int Xclose(int sockfd)
{
	xia::XSocketCallType type;
	int rc;

	if (getSocketType(sockfd) == XSOCK_INVALID)
	{
		LOG("The socket is not a valid Xsocket");
		errno = EBADF;
		return -1;
	}

	xia::XSocketMsg xsm;
	xsm.set_type(xia::XCLOSE);

	if ((rc = click_control(sockfd, &xsm)) < 0) {
		LOGF("Error talking to Click: %s", strerror(errno));

	} else if ((rc = click_reply2(sockfd, &type)) < 0) {
		LOGF("Error getting status from Click: %s", strerror(errno));
	}

	close(sockfd);
	freeSocketState(sockfd);
	return rc;
}

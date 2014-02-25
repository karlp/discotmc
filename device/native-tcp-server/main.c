/*-
 * Copyright (c) 2012-2013 Jan Breuer,
 *
 * All Rights Reserved
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file   main.c
 * @date   Thu Nov 15 10:58:45 UTC 2012
 * 
 * @brief  TCP/IP SCPI Server
 * 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <arpa/inet.h>

#include <scpi/scpi.h>
#include "dscpi.h"
#include "funcgen.h"



static int createServer(int port) {
    int fd;
    int rc;
    int on = 1;
    struct sockaddr_in servaddr;
        
    /* Configure TCP Server */
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(port);
    
    /* Create socket */
    fd = socket(AF_INET,SOCK_STREAM, 0);
    if (fd < 0)
    {
        perror("socket() failed");
        exit(-1);
    }    
    
    /* Set address reuse enable */
    rc = setsockopt(fd, SOL_SOCKET,  SO_REUSEADDR, (char *)&on, sizeof(on));
    if (rc < 0)
    {
        perror("setsockopt() failed");
        close(fd);
        exit(-1);
    }
   
    /* Set non blocking */
    rc = ioctl(fd, FIONBIO, (char *)&on);
    if (rc < 0)
    {
        perror("ioctl() failed");
        close(fd);
        exit(-1);
    }    
    
    /* Bind to socket */
    rc = bind(fd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (rc < 0)
    {
        perror("bind() failed");
        close(fd);
        exit(-1);
    }
    
    /* Listen on socket */
    listen(fd, 1);
    if (rc < 0)
    {
        perror("listen() failed");
        close(fd);
        exit(-1);
    }
    
    return fd;
}

static int waitServer(int fd) {
    fd_set fds;
    struct timeval timeout;
    int rc;
    int max_fd;
    
    FD_ZERO(&fds);
    max_fd = fd;
    FD_SET(fd, &fds);
    
    timeout.tv_sec  = 5;
    timeout.tv_usec = 0;
    
    rc = select(max_fd + 1, &fds, NULL, NULL, &timeout);
    
    return rc;
}

/*
 * 
 */
int main(int argc, char** argv) {
    (void) argc;
    (void) argv;
    int rc;

    int listenfd;
    uint8_t smbuffer[40];

    // user_context will be pointer to socket
    //scpi_context.user_context = NULL;
    
    scpi_init_platform("12345678");

    listenfd = createServer(5025);
    printf("Disco non stop TMC! (listening for SCPI commands on port: 5025)\n");
    
    while(1) {
        int clifd;
        struct sockaddr_in cliaddr;
        socklen_t clilen;

        clilen = sizeof(cliaddr);
        clifd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
        
        if (clifd < 0) continue;

        printf("Connection established %s\r\n", inet_ntoa(cliaddr.sin_addr));

        //scpi_context.user_context = &clifd;
	dscpi_get_context()->user_context = &clifd;

        while(1) {
            rc = waitServer(clifd);
            if (rc < 0) { // failed
                perror("  recv() failed");
                break;
            }
            if (rc == 0) { // timeout
                	scpi_glue_input(NULL, 0, false);
            }
            if (rc > 0) { // something to read
                rc = recv(clifd, smbuffer, sizeof(smbuffer), 0);
                if (rc < 0) {
                    if (errno != EWOULDBLOCK) {
                        perror("  recv() failed");
                        break;
                    }
                } else if (rc == 0) {                
                    printf("Connection closed\r\n");
                    break;
                } else {
			scpi_glue_input(smbuffer, rc, false);
                }
            }
        }

        close(clifd);
    }
    
    return (EXIT_SUCCESS);
}

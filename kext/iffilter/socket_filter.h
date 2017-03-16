// https://developer.apple.com/library/content/documentation/Darwin/Conceptual/NKEConceptual/socket_nke/socket_nke.html#//apple_ref/doc/uid/TP40001858-CH228-SW1

#ifndef socket_filter_h
#define socket_filter_h

#include "common.h"
#include <sys/kpi_socketfilter.h> // includes functions and data type definitions for creating a socket filter.
#include <sys/kpi_socket.h> // includes functions for manipulating a socket, including packet send/receive and flag manipulation
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/sockio.h>
#include <net/kpi_protocol.h> // includes functions for packet injection. It also includes functions to register “plumbers”—handlers that deal with requests for attaching a protocol to an interface (and detaching, and so on).

extern int user_pid;

extern int socket_status;

/**
 * input output byte limit for socket filter
 *
 * 0 is unlimted or no limit (default)
 **/
extern int user_socket_io_byte_limit;
/**
 * input byte limit for socket filter
 *
 * 0 is unlimted or no limit (default)
 **/
extern int user_socket_in_byte_limit;
/**
 * output byte limit for socket filter
 *
 * 0 is unlimted or no limit (default)
 **/
extern int user_socket_out_byte_limit;

/**
 * initial setup for the socket filter
 * @return 0 on success
 **/
kern_return_t socket_filter_setup();

/**
 * final cleanup for the socket filter
 * @return 0 on success
 **/
kern_return_t socket_filter_cleanup();

/**
 * must call socket_filter_stop before exiting
 * @result 0 on success, error otherwise
 **/
errno_t socket_filter_start();

/**
 * @result 0 on success, error otherwise
 **/
errno_t socket_filter_stop();

/**
 * status of filtering for sockets
 * can be used to start or stop the interface filter
 * or see if the filter is currently active
 *
 *takes an integer as an argument
 * @param oidp id
 * @param arg1 first argument
 * @param arg2 second argument
 * @param req the request
 **/
int sysctl_socket_status_handler SYSCTL_HANDLER_ARGS;

#endif /* socket_filter_h */

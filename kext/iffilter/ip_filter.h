// https://developer.apple.com/library/content/documentation/Darwin/Conceptual/NKEConceptual/ip_filter_nke/ip_filter_nke.html#//apple_ref/doc/uid/TP40001858-CH229-SW1

#ifndef ip_filter_h
#define ip_filter_h

#include "common.h"
#include <netinet/kpi_ipfilter.h> // includes functions for attaching a packet filter for IPv4 or IPv6 packets. These functions are appropriate for a KEXT that filters IP traffic.
#include <netinet/ip.h>
#include <netinet/ip6.h>

extern int ip_status;

/**
 * input output byte limit for interface filter
 *
 * 0 is unlimted or no limit (default)
 **/
extern int user_ip_io_byte_limit;
/**
 * input byte limit for interface filter
 *
 * 0 is unlimted or no limit (default)
 **/
extern int user_ip_in_byte_limit;
/**
 * output byte limit for interface filter
 *
 * 0 is unlimted or no limit (default)
 **/
extern int user_ip_out_byte_limit;

/**
 * initial setup for the ip filter
 * @return 0 on success
 **/
kern_return_t ip_filter_setup();

/**
 * final cleanup for the ip filter
 * @return 0 on success
 **/
kern_return_t ip_filter_cleanup();

/**
 * must call ip_filter_stop before exiting
 * @result 0 on success, error otherwise
 **/
errno_t ip_filter_start();

/**
 * @result 0 on success, error otherwise
 **/
errno_t ip_filter_stop();

/**
 * status of filtering for ip
 * can be used to start or stop the interface filter
 * or see if the filter is currently active
 *
 *takes an integer as an argument
 * @param oidp id
 * @param arg1 first argument
 * @param arg2 second argument
 * @param req the request
 **/
int sysctl_ip_status_handler SYSCTL_HANDLER_ARGS;

/**
 * `in_addr` or `in6_addr` for the ip filter
 * @param oidp id
 * @param arg1 first argument
 * @param arg2 second argument
 * @param req the request
 **/
int sysctl_ip_addr_handler SYSCTL_HANDLER_ARGS;

#endif /* ip_filter_h */

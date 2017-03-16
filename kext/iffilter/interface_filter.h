// https://developer.apple.com/library/content/documentation/Darwin/Conceptual/NKEConceptual/interface_filter_nke/interface_filter_nke.html#//apple_ref/doc/uid/TP40001858-CH230-SW1

#ifndef interface_filter_h
#define interface_filter_h

#include "common.h"
#include <net/kpi_interfacefilter.h> // includes functions for filtering at the raw packet level, just above the network interface layer. These functions are appropriate for an interface filter.

struct filters {
    int id;
    interface_filter_t ref;
    struct iff_filter filter;
    
    struct kev_msg *last_event_msg;
    unsigned long last_ioctl_cmd;
    void * last_ioctl_arg;
    mbuf_t *last_in_data;
    char **last_frame_ptr;
    mbuf_t *last_out_data;
    
    time_t reset;
    u_int64_t bytes_out_since_reset;
    u_int64_t bytes_in_since_reset;
};

/** 
 * input output byte limit for interface filter
 *
 * 0 is unlimted or no limit (default)
 **/
extern int user_interface_io_byte_limit;
/** 
 * input byte limit for interface filter
 *
 * 0 is unlimted or no limit (default)
 **/
extern int user_interface_in_byte_limit;
/** 
 * output byte limit for interface filter
 *
 * 0 is unlimted or no limit (default)
 **/
extern int user_interface_out_byte_limit;

/**
 * flag to determine if currently filtering interfaces
 **/
extern int interface_status;

/**
 * iff_ioctl_func is used to filter ioctls sent to an interface. The interface is only valid for the
 * duration of the filter call. If you need to keep a reference to the interface, be sure to call
 * ifnet_reference and ifnet_release.
 *
 * All undefined ioctls are reserved for future use by Apple. If you need to communicate with your
 * kext using an ioctl, please use SIOCSIFKPI and SIOCGIFKPI.
 *
 * @param cookie The cookie specified when this filter was attached.
 * @param interface The interface the packet is being transmitted on.
 * @param protocol The protocol of this packet. If you specified a protocol when
 *      attaching your filter, the protocol will only ever be the protocol you specified.
 * @param ioctl_cmd The ioctl command.
 * @param ioctl_arg A pointer to the ioctl argument.
 *
 * @result 0 - This filter function handled the ioctl. EOPNOTSUPP - This filter function does not
 *      understand/did not handle this ioctl. EJUSTRETURN - This filter function handled the ioctl,
 *      processing should stop. Anything Else - Processing will stop, the error will be returned.
 **/
errno_t ioctl_handler(void *cookie, ifnet_t interface, protocol_family_t protocol, unsigned long ioctl_cmd, void *ioctl_arg);

/**
 * iff_event_func is used to filter interface specific events. The interface is only valid for the
 * duration of the filter call. If you need to keep a reference to the interface, be sure to call
 * ifnet_reference and ifnet_release.
 *
 * @param cookie The cookie specified when this filter was attached.
 * @param interface The interface the packet is being transmitted on.
 * @param protocol The protocol of this packet. If you specified a protocol when
 *      attaching your filter, the protocol will only ever be the protocol you specified.
 * @param event_msg The kernel event, may not be changed.
 **/
void event_handler(void *cookie, ifnet_t interface, protocol_family_t protocol, const struct kev_msg *event_msg);

/**
 * iff_output_func is used to filter fully formed outbound packets. The interface is only valid
 * for the duration of the filter call. If you need to keep a reference to the interface, be sure
 * to call ifnet_reference and ifnet_release.
 *
 * @param cookie The cookie specified when this filter was attached.
 * @param interface The interface the packet is being transmitted on.
 * @param protocol The protocol of this packet. If you specified a protocol when
 *      attaching your filter, the protocol will only ever be the protocol you specified.
 * @param data The fully formed outbound packet in a chain of mbufs. The frame header is
 *      already included. The filter function may modify the packet or return a different mbuf chain.
 *
 * @result 0 - The caller will continue with normal processing of the packet.
 *      EJUSTRETURN - The caller will stop processing the packet, the packet will not be freed.
 *      Anything Else - The caller will free the packet and stop processing.
 **/
errno_t out_handler(void *cookie, ifnet_t interface, protocol_family_t protocol, mbuf_t *data);

/**
 * iff_input_func is used to filter incoming packets. The interface is only valid for the
 * duration of the filter call. If you need to keep a reference to the interface, be sure
 * to call ifnet_reference and ifnet_release. The packets passed to the inbound filter are
 * different from those passed to the outbound filter. Packets to the inbound filter have
 * the frame header passed in separately from the rest of the packet. The outbound data
 * filters is passed the whole packet including the frame header.
 *
 * The frame header usually preceeds the data in the mbuf. This ensures that the frame header
 * will be a valid pointer as long as the mbuf is not freed. If you need to change the frame
 * header to point somewhere else, the recommended method is to prepend a new frame header to
 * the mbuf chain (mbuf_prepend), set the header to point to that data, then call mbuf_adj to
 * move the mbuf data pointer back to the start of the packet payload.
 *
 * @param cookie The cookie specified when this filter was attached.
 * @param interface The interface the packet was recieved on.
 * @param protocol The protocol of this packet. If you specified a protocol when
 *      attaching your filter, the protocol will only ever be the protocol you specified.
 * @param data The inbound packet, after the frame header as determined by the interface.
 * @param frame_ptr A pointer to the pointer to the frame header. The frame header
 *      length can be found by inspecting the interface's frame header length (ifnet_hdrlen).
 *
 * @result 0 - The caller will continue with normal processing of the packet.
 *      EJUSTRETURN - The caller will stop processing the packet, the packet will not be freed.
 *      Anything Else - The caller will free the packet and stop processing.
 *
 **/
errno_t in_handler(void *cookie, ifnet_t interface, protocol_family_t protocol, mbuf_t *data, char **frame_ptr);

/**
 * must call interface_filter_stop before exiting
 * @result 0 on success, error otherwise
 **/
errno_t interface_filter_start();

/**
 * @result 0 on success, error otherwise
 **/
errno_t interface_filter_stop();

/**
 * set the interfaces
 * handler for settings the user-defined interface list
 * @param oidp id
 * @param arg1 first argument
 * @param arg2 second argument
 * @param req the request
 **/
int sysctl_iff_interfaces_handler SYSCTL_HANDLER_ARGS;

/**
 * status of filtering for interfaces
 * can be used to start or stop the interface filter
 * or see if the filter is currently active
 * 
 * takes an integer as an argument
 * @param oidp id
 * @param arg1 first argument
 * @param arg2 second argument
 * @param req the request
 **/
int sysctl_iff_status_handler SYSCTL_HANDLER_ARGS;

/** 
 * frees user defined interface list
 * @return 0 on success
 **/
int freeUserInterfaces();

/**
 * initial setup for the interface filter
 * @return 0 on success
 **/
kern_return_t interface_filter_setup();

/**
 * final cleanup for the interface filter
 * @return 0 on success
 **/
kern_return_t interface_filter_cleanup();

#endif /* interface_filter_h */

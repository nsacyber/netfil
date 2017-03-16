#include "socket_filter.h"

static sflt_handle tcp4Handler = 'tcp4';
static sflt_handle tcp6Handler = 'tcp6';

// TODO: filter every tother thing
static sflt_handle udp4Handler = 'udp4';
static sflt_handle udp6Handler = 'udp6';
static sflt_handle icmp4Handler= 'icm4';
static sflt_handle icmp6Handler= 'icm6';
static sflt_handle raw4Handler = 'rip4';
static sflt_handle raw6Handler = 'rip4';

static int filtersAreRegistered = 0;

static int bytes_out_since_reset = 0;
static int bytes_in_since_reset = 0;

static errno_t
sip_attach(
           void		**cookie,
           socket_t	socket)
{
    DPRINT("attached to a socket\n");
    
    DPRINT("self socket pid %d\n", proc_selfpid());
    
    errno_t ret = 0;
    
    // pid is not desired so don't need the filter...
    if(user_pid != proc_selfpid()) { // TODO: proc_selfpid may not be accurate
        ret = ENXIO;                 // See: https://lists.apple.com/archives/darwin-kernel/2006/Oct/msg00166.html
    }
    
    return ret;
}

static void
sip_detach(
           void		*cookie,
           socket_t	socket)
{
    DPRINT("detached socket");
}

static void
sip_unregistered(
                 sflt_handle handle)
{
    DPRINT("unregistered socket");
}

static errno_t
sip_sock_out(
             void					*cookie,
             socket_t				so,
             const struct sockaddr   *to,
             mbuf_t					*data,
             mbuf_t					*control,
             sflt_data_flag_t		flags)
{
    DPRINT("socket data out %zu\n", mbuf_len(*data));
    
    bytes_out_since_reset += mbuf_len(*data);
    
    if(user_socket_out_byte_limit != 0 && bytes_out_since_reset > user_socket_out_byte_limit) {
        return !EJUSTRETURN;
    }
    
    if(user_socket_io_byte_limit != 0 && (bytes_in_since_reset + bytes_out_since_reset) > user_socket_io_byte_limit) {
        return !EJUSTRETURN;
    }
    
    return 0;
}

static errno_t sip_sock_in(void *cookie, socket_t so, const struct sockaddr *from, mbuf_t *data, mbuf_t *control, sflt_data_flag_t flags) {
    DPRINT("socket data in %zu\n", mbuf_len(*data));
    
    bytes_in_since_reset += mbuf_len(*data);
    
    if(user_socket_in_byte_limit != 0 && bytes_in_since_reset > user_socket_in_byte_limit) {
        return !EJUSTRETURN;
    }
    
    if(user_socket_io_byte_limit != 0 && (bytes_in_since_reset + bytes_out_since_reset) > user_socket_io_byte_limit) {
        return !EJUSTRETURN;
    }
    
    return 0;
}

errno_t socket_filter_setup() {
    errno_t ret = 0;
    
    user_pid = 0;
    
    socket_status =  0;
    
    return ret;
}

errno_t socket_filter_start() {
    errno_t ret = 0;

    if(filtersAreRegistered == 1) {
        return -1;
    }
    
    struct sflt_filter tcpFilter;
    bzero(&tcpFilter, sizeof(tcpFilter));
    tcpFilter.sf_name = "com.company.netfil tcp ipv4 filter";
    tcpFilter.sf_handle = tcp4Handler;
    tcpFilter.sf_flags = SFLT_GLOBAL;
    tcpFilter.sf_attach = sip_attach;
    tcpFilter.sf_detach = sip_detach;
    tcpFilter.sf_unregistered = sip_unregistered;
    tcpFilter.sf_data_out = sip_sock_out;
    tcpFilter.sf_data_in = sip_sock_in;
    ret |= sflt_register(&tcpFilter, PF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    if(ret != 0) {
        DPRINT("failed to start ipv4 socket filter\n");
    }
    
    tcpFilter.sf_name = "com.company.netfil tcp ipv6 filter";
    tcpFilter.sf_handle = tcp6Handler;
    ret |= sflt_register(&tcpFilter, PF_INET6, SOCK_STREAM, IPPROTO_TCP);
    
    if(ret != 0) {
        DPRINT("failed to start ipv6 socket filter\n");
    }
    
    socket_status = 1;
    filtersAreRegistered = 1;
    
    if(ret != 0) {
        socket_filter_cleanup();
    }
    
    return ret;

}

errno_t socket_filter_stop() {
    errno_t ret = 0;
    ret = socket_filter_cleanup();
    
    bytes_out_since_reset = 0;
    bytes_in_since_reset = 0;
    
    return ret;
}


errno_t socket_filter_cleanup() {
    errno_t ret = 0;
    
    ret = sflt_unregister(tcp4Handler);
    ret = sflt_unregister(tcp6Handler);
    filtersAreRegistered = 0;
    
//    ret = sflt_unregister(udp4Handler);
//    ret = sflt_unregister(udp6Handler);
//    
//    ret = sflt_unregister(icmp4Handler);
//    ret = sflt_unregister(icmp6Handler);
//    
//    ret = sflt_unregister(raw4Handler);
//    ret = sflt_unregister(raw6Handler);
        
    socket_status = 0;
        
    return ret;
}

int sysctl_socket_status_handler SYSCTL_HANDLER_ARGS {
    DPRINT("inside %s (%d)\n", oidp->oid_name, oidp->oid_number);
        
    int old = socket_status;
    
    int error = sysctl_handle_int(oidp, arg1, arg2, req);
    
    if (!error && req->newptr) { // write request
        DPRINT("write for %s %d\n", oidp->oid_name, error);
        DPRINT("old %d new %d\n", old, socket_status);
        
        if(old != socket_status) {
            DPRINT("things are different\n");
            if(socket_status == 1) {
                if(socket_filter_start() != 0) {
                    // some error, cleanup and set status to 0
                    DPRINT("failed to start filter so stopping\n");
                    socket_filter_stop();
                    socket_status = 0;
                }
            } else {
                socket_status = 0;
                socket_filter_stop();
            }
        }
    } else if (req->newptr) {
        DPRINT("write with error %d for %s\n", error, oidp->oid_name);
        /* Something was wrong with the write request */
        /* Do something here if you feel like it.... */
    } else {
        DPRINT("read for socket.%s=%d, %d\n", oidp->oid_name, socket_status, error);
        error = 0;
    }
    
    return error;
}

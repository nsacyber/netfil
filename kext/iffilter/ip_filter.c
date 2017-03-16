#include "ip_filter.h"

static ipfilter_t ipv4Filter;
static u_int32_t ipv4COOKIE = 'ipv4';


static ipfilter_t ipv6Filter;
static u_int32_t ipv6COOKIE = 'ipv6';

static struct in_addr ipv4Addr = {0};
static struct in6_addr ipv6Addr = {0};

static int bytes_out_since_reset = 0;
static int bytes_in_since_reset = 0;

void ipf_detach_handler(void *cookie) {
}

kern_return_t ip_filter_setup() {
    kern_return_t ret = KERN_SUCCESS;
    
    ipv4Filter = NULL;
    ipv6Filter = NULL;
    
    return ret;
}

errno_t ipf_input_handler(void *cookie, mbuf_t *data, int offset, u_int8_t protocol) {
    bytes_in_since_reset += mbuf_len(*data);
    
    char srcStr[INET6_ADDRSTRLEN] = {0};
    char dstStr[INET6_ADDRSTRLEN] = {0};
    
    if(cookie == &ipv4COOKIE) {
        //DPRINT("ipv4 cookie!\n");
        if(mbuf_len(*data) >= sizeof(struct ip)) {
            struct ip *iii = (struct ip*) data;
            inet_ntop(AF_INET, &(iii->ip_dst), dstStr, INET_ADDRSTRLEN);
            inet_ntop(AF_INET, &(iii->ip_src), srcStr, INET_ADDRSTRLEN);
            //iii->ip_dst;
            //iii->ip_src;
        }
    } else if(cookie == &ipv6COOKIE){
        //DPRINT("ipv6 cookie!\n");
        if(mbuf_len(*data) >= sizeof(struct ip6_hdr)) {
            struct ip6_hdr *iii = (struct ip6_hdr *) data;
            //iii->ip6_dst;
            //iii->ip6_src;
            inet_ntop(AF_INET6, &(iii->ip6_dst), dstStr, INET6_ADDRSTRLEN);
            inet_ntop(AF_INET6, &(iii->ip6_src), srcStr, INET6_ADDRSTRLEN);
        }
    } else {
        DPRINT("Unknown cookie! %p\n", cookie);
        return 0;
    }
    
    //DPRINT("src %s; dst %s", srcStr, dstStr);
    
    
    
    
    if(user_ip_in_byte_limit != 0 && bytes_in_since_reset > user_ip_in_byte_limit) {
        return !EJUSTRETURN;
    }
    
    if(user_ip_io_byte_limit != 0 && (bytes_in_since_reset + bytes_out_since_reset) > user_ip_io_byte_limit) {
        return !EJUSTRETURN;
    }
    
    return 0;
};

errno_t ipf_output_handler(void *cookie, mbuf_t *data, ipf_pktopts_t options) {
    bytes_out_since_reset += mbuf_len(*data);
    
    char srcStr[INET6_ADDRSTRLEN] = {0};
    char dstStr[INET6_ADDRSTRLEN] = {0};
    
    if(cookie == &ipv4COOKIE) {
        //DPRINT("ipv4 cookie!\n");
        if(mbuf_len(*data) >= sizeof(struct ip)) {
            struct ip *iii = (struct ip*) data;
            inet_ntop(AF_INET, &(iii->ip_dst), dstStr, INET_ADDRSTRLEN);
            inet_ntop(AF_INET, &(iii->ip_src), srcStr, INET_ADDRSTRLEN);
        }
    } else if(cookie == &ipv6COOKIE){
        //DPRINT("ipv6 cookie!\n");
        if(mbuf_len(*data) >= sizeof(struct ip6_hdr)) {
            struct ip6_hdr *iii = (struct ip6_hdr *) data;
            //iii->ip6_dst;
            //iii->ip6_src;
            inet_ntop(AF_INET6, &(iii->ip6_dst), dstStr, INET6_ADDRSTRLEN);
            inet_ntop(AF_INET6, &(iii->ip6_src), srcStr, INET6_ADDRSTRLEN);
        }
    } else {
        DPRINT("Unknown cookie! %p\n", cookie);
        return 0;
    }
    
    //DPRINT("src %s; dst %s", srcStr, dstStr);
    
    if(user_ip_out_byte_limit != 0 && bytes_out_since_reset > user_ip_out_byte_limit) {
        return !EJUSTRETURN;
    }
    
    if(user_ip_io_byte_limit != 0 && (bytes_in_since_reset + bytes_out_since_reset) > user_ip_io_byte_limit) {
        return !EJUSTRETURN;
    }
    
    return 0;
}

/**
 * final cleanup for the ip filter
 * @return 0 on success
 **/
kern_return_t ip_filter_cleanup() {
    kern_return_t ret = KERN_SUCCESS;
    
    ret |= ip_filter_stop();
    
    return ret;
}

errno_t ip_filter_start() {
    errno_t ret = KERN_SUCCESS;
    
    bytes_out_since_reset = 0;
    bytes_in_since_reset = 0;
    
    //struct ipf_fiter ipff;
    struct ipf_filter ipff;
    ipff.ipf_detach = ipf_detach_handler;
    ipff.ipf_input = ipf_input_handler;
    ipff.ipf_output = ipf_output_handler;
    
    ipff.cookie = &ipv4COOKIE;
    ipff.name = "netfil ipv4 filter";
    ret |= ipf_addv4(&ipff, &ipv4Filter);
    
    ipff.cookie = &ipv6COOKIE;
    ipff.name = "netfil ipv6 filter";
    ret |= ipf_addv6(&ipff, &ipv6Filter);
    
    ip_status = 1;
    
    return ret;
}
errno_t ip_filter_stop() {
    errno_t ret = KERN_SUCCESS;
    
    ipf_remove(ipv4Filter);
    ipf_remove(ipv6Filter);
    
    ipv4Filter = NULL;
    ipv6Filter = NULL;
    
    bytes_out_since_reset = 0;
    bytes_in_since_reset = 0;
    
    bzero(&ipv4Addr, sizeof(ipv4Addr));
    bzero(&ipv6Addr, sizeof(ipv6Addr));
    
    ip_status = 0;
    return ret;
}

int sysctl_ip_addr_handler SYSCTL_HANDLER_ARGS {
    DPRINT("inside %s (%d)\n", oidp->oid_name, oidp->oid_number);
    
    struct in_addr r;
    int error = SYSCTL_IN(req, &r, sizeof(r));
    
    if(error) {
        struct in6_addr r2;
        error = SYSCTL_IN(req, &r2, sizeof(r2));
    }
    
    // TODO: do a check for just a string... inet_pton
    
    if (!error && req->newptr) {
        DPRINT("size of ip addr %zu\n", req->newlen);
        
        bzero(&ipv4Addr, sizeof(ipv4Addr));
        bzero(&ipv6Addr, sizeof(ipv6Addr));
        
        if(req->newlen == sizeof(struct in_addr)) {
            error = copyin(req->newptr, &ipv4Addr, sizeof(struct in_addr));
            
            char srcStr[INET6_ADDRSTRLEN] = {0};
            inet_ntop(AF_INET, &ipv4Addr, srcStr, INET_ADDRSTRLEN);
            DPRINT("new ip? %s\n", srcStr);
            
        } else if(req->newlen == sizeof(struct in6_addr)) {
            error = copyin(req->newptr, &ipv6Addr, sizeof(struct in6_addr));
            
        } else {
            DPRINT("unknown size of user data\n");
            return -1;
        }
    } else if (req->newptr) {
        /* Something was wrong with the write request */
        /* Do something here if you feel like it.... */
        DPRINT("some error when copying... %d %zu\n", error, req->newlen);
    } else {
        /* Read request. Always return 763, just for grins. */
        DPRINT("sysctl %s: read\n", oidp->oid_name);
    
        if(req->oldptr && req->oldlen == sizeof(struct in_addr)) {
            SYSCTL_OUT(req, &ipv4Addr, sizeof(struct in_addr));
        } else if(req->oldptr && req->oldlen == sizeof(struct in6_addr)) {
            SYSCTL_OUT(req, &ipv6Addr, sizeof(struct in6_addr));
        }
        
    }
    
    return 0;
}

int sysctl_ip_status_handler SYSCTL_HANDLER_ARGS {
    DPRINT("inside ip.%s (%d)\n", oidp->oid_name, oidp->oid_number);
    
    int old = ip_status;
    
    int error = sysctl_handle_int(oidp, arg1, arg2, req);
    
    if (!error && req->newptr) { // write request
        DPRINT("write for ip.%s %d\n", oidp->oid_name, error);
        DPRINT("old %d new %d\n", old, ip_status);
        
        if(old != ip_status) {
            DPRINT("things are different\n");
            if(ip_status == 1) {
                if(ip_filter_start() != 0) {
                    // some error, cleanup and set status to 0
                    ip_filter_stop();
                    ip_status = 0;
                }
            } else {
                ip_status = 0;
                ip_filter_stop();
            }
        }
    } else if (req->newptr) {
        DPRINT("write with error %d for %s\n", error, oidp->oid_name);
        /* Something was wrong with the write request */
        /* Do something here if you feel like it.... */
    } else {
        DPRINT("read for ip.%s=%d %d\n", oidp->oid_name, ip_status, error);
        error = 0;
    }
    
    return error;
}

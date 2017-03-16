#include "interface_filter.h"

/** interfaces to filter */
static ifnet_t *interfaces = NULL;
/** active filters of interfaces */
static struct filters *filters = NULL;
/** number of interfaces to filter */
static u_int32_t ifCount = 0;
/** number of interfaces currenly with a filter */
static u_int32_t realIfCount = 0;
/** user defined interface list */
static struct array *userInterfaces = NULL;

errno_t interface_filter_setup() {
    interfaces = NULL;
    user_interface_io_byte_limit = 0;
    user_interface_in_byte_limit = 0;
    user_interface_out_byte_limit = 0;
    interface_status = 0;
    return 0;
}

errno_t interface_filter_cleanup() {
    errno_t ret = 0;
    
    ret = interface_filter_stop();
    
    return ret;
}

errno_t interface_filter_start() {
    
    if(interfaces != NULL) {
        // this was already called so don't do it again!
        DPRINT("WARNING! You have to stop, before you can start again!");
        return -1;
    }
    
    errno_t ret = 0;
    
    ifCount = 0;
    
    if(userInterfaces) {
        #ifdef DEBUG
            DPRINT("userInteface count %d\n", userInterfaces->size);
        #endif
        if(userInterfaces->size > 0) {
            interfaces = OSMalloc(sizeof(interfaces) * userInterfaces->size, globalOSMallocTag);
            if(interfaces == NULL) {
                DPRINT("unable to malloc interfaces\n");
                return -1;
            }
            for(int i = 0; i < userInterfaces->size; i++) {
                ifnet_t p_ifnet;
                errno_t err = ifnet_find_by_name(userInterfaces->values[i], &p_ifnet);
                if(err) {
                    DPRINT("%s is not a valid interface\n", userInterfaces->values[i]);
                    continue;
                }
                interfaces[ifCount++] = p_ifnet;
            }
        }
    }
    
    DPRINT("ifCount = %d\n", ifCount);
    
    if(ifCount <= 0){
        if(interfaces) { // userInterfaces was not null but no valid interfaces... so have to free
            OSFree(interfaces, sizeof(interfaces) * userInterfaces->size, globalOSMallocTag);
        }
        ret = ifnet_list_get(IFNET_FAMILY_ANY, &interfaces, &ifCount);
        if(ret != 0) {
            return -1;
        }
        DPRINT("got list of %d attached interfaces \n", ifCount);
        
    }
    
    if(ifCount > 0) {
        filters = OSMalloc(sizeof(struct filters) * ifCount, globalOSMallocTag);
        if(filters == NULL) {
            ifnet_list_free(interfaces);
            DPRINT("malloc error\n");
            return -2;
        }
    }
    
    DPRINT("malloc success\n");
    
    for(int i = 0; i < ifCount; i++) {
        const char *name = ifnet_name(interfaces[i]);
        
        DPRINT("at %s %d\n", name, i);
        
        if(strncmp(name, "lo", 2) == 0)
            continue;
        
        filters[realIfCount].id = realIfCount;
        filters[realIfCount].filter.iff_cookie   = (void *) &(filters[realIfCount]);
        filters[realIfCount].filter.iff_name     = "iffilter";
        filters[realIfCount].filter.iff_output   = out_handler;
        filters[realIfCount].filter.iff_input    = in_handler;
        filters[realIfCount].filter.iff_protocol = 0;
        filters[realIfCount].filter.iff_event    = event_handler;
        filters[realIfCount].filter.iff_ioctl    = ioctl_handler;
        filters[realIfCount].filter.iff_detached = NULL;
        
        struct timespec tsp;
        getnanotime(&tsp);
        filters[realIfCount].reset = tsp.tv_sec;
        filters[realIfCount].bytes_in_since_reset = 0;
        filters[realIfCount].bytes_out_since_reset = 0;
        
        ret = iflt_attach(interfaces[i], &(filters[realIfCount].filter), &(filters[realIfCount].ref));
        if(ret == 0) {
            DPRINT("an interface registered\n");
            realIfCount++;
        }
    }
    
    return ret;
}

errno_t interface_filter_stop() {
    
    if(interfaces) ifnet_list_free(interfaces);
    
    if(filters) {
        for(int i = 0; i < realIfCount; i++) {
            if(filters[i].ref != NULL) {
                iflt_detach(filters[i].ref);
            }
        }
        OSFree(filters, sizeof(struct filters) * ifCount, globalOSMallocTag);
    }
    
    freeUserInterfaces();
    
    filters = NULL;
    interfaces = NULL;
    ifCount = 0;
    realIfCount = 0;
    
    interface_status = 0;
    
    return 0;
}

errno_t ioctl_handler(void *cookie, ifnet_t interface, protocol_family_t protocol, unsigned long ioctl_cmd, void *ioctl_arg) {
    #ifdef DEBUG
        const char *name = ifnet_name(interface);
        u_int32_t index = ifnet_index(interface);
        DPRINT("filtering ioctl for %s%d\n", name, index);
    #endif
    return 0;
}

void event_handler(void *cookie, ifnet_t interface, protocol_family_t protocol, const struct kev_msg *event_msg) {
    #ifdef DEBUG
        const char *name = ifnet_name(interface);
        u_int32_t index = ifnet_index(interface);
        DPRINT("%s%d received event [vendor_code = %u, kev_class = %u, kev_subclass = %u, event_code = %u]\n", name, index, event_msg->vendor_code, event_msg->kev_class, event_msg->kev_subclass, event_msg->event_code);
    #endif
}

errno_t out_handler(void *cookie, ifnet_t interface, protocol_family_t protocol, mbuf_t *data) {
    
    struct filters *filter = (struct filters *) cookie;
    filter->bytes_out_since_reset += mbuf_len(*data);
    
    
    #ifdef DEBUG
        const char *name = ifnet_name(interface);
        u_int32_t index = ifnet_index(interface);
        DPRINT("filtering out for %s%d\n", name, index);
        DPRINT("bytes_out_since_reset: %llu\n",filter->bytes_out_since_reset);
    #endif
    
    if(user_interface_out_byte_limit != 0 && filter->bytes_out_since_reset > user_interface_out_byte_limit) {
        return !EJUSTRETURN;
    }
    
    if(user_interface_io_byte_limit != 0 && (filter->bytes_in_since_reset + filter->bytes_out_since_reset) > user_interface_io_byte_limit) {
        return !EJUSTRETURN;
    }
    
    return 0;
}

errno_t in_handler(void *cookie, ifnet_t interface, protocol_family_t protocol, mbuf_t *data, char **frame_ptr) {
    
    DPRINT("self iface pid %d\n", proc_selfpid());
    DPRINT("self iface uid %d\n", kauth_getuid() );
    
    struct filters *filter = (struct filters *) cookie;
    filter->bytes_in_since_reset += mbuf_len(*data);
    
    #ifdef DEBUG
        const char *name = ifnet_name(interface);
        u_int8_t type = ifnet_type(interface);
        u_int32_t mtu = ifnet_mtu(interface); // maximum transmission unit
        u_int32_t unit = ifnet_unit(interface); // unit number
        u_int32_t index = ifnet_index(interface); // This index value will match the index you would find in a sockaddr_dl or using if_nametoindex or if_indextoname in user space. The value of the interface index is undefined for an interface that is not currently attached.
        
        DPRINT("filtering in for %s with type %d; mtu %d; unit %d; index %d\n", name, type, mtu, unit, index);
        DPRINT("protocol: %d\n", protocol);
        
        struct ifnet_stats_param stats;
        ifnet_stat(interface, &stats);
        DPRINT("bytes_in %llu; bytes_out %llu; packets in %llu; packets_out %llu\n", stats.bytes_in, stats.bytes_out, stats.packets_in, stats.packets_out);
        DPRINT("bytes_in_since_reset: %llu\n", filter->bytes_in_since_reset);
    #endif
    
    if(user_interface_in_byte_limit != 0 && filter->bytes_in_since_reset > user_interface_in_byte_limit) {
        return !EJUSTRETURN;
    }
    
    if(user_interface_io_byte_limit != 0 && (filter->bytes_in_since_reset + filter->bytes_out_since_reset) > user_interface_io_byte_limit) {
        return !EJUSTRETURN;
    }
    
    return 0;
}

int sysctl_iff_status_handler SYSCTL_HANDLER_ARGS {
    DPRINT("inside interface %s (%d)\n", oidp->oid_name, oidp->oid_number);
    
    int old = interface_status;
    
    int error = sysctl_handle_int(oidp, arg1, arg2, req);
    
    if (!error && req->newptr) { // write request
        DPRINT("write for %s %d\n", oidp->oid_name, error);
        DPRINT("old %d new %d\n", old, interface_status);

        if(old != interface_status) {
            DPRINT("things are different\n");
            if(interface_status == 1) {
                if(interface_filter_start() != 0) {
                    // some error, cleanup and set status to 0
                    interface_filter_stop();
                    interface_status = 0;
                }
            } else {
                interface_status = 0;
                interface_filter_stop();
            }
        }
    } else if (req->newptr) {
        DPRINT("write with error %d for %s\n", error, oidp->oid_name);
        /* Something was wrong with the write request */
        /* Do something here if you feel like it.... */
    } else {
        DPRINT("read for interface %s: %d\n", oidp->oid_name, interface_status);
        error = 0;
    }
    
    return error;
}

int sysctl_iff_interfaces_handler SYSCTL_HANDLER_ARGS {
    DPRINT("inside interface.%s (%d)\n", oidp->oid_name, oidp->oid_number);
    
    uint64_t addr;
    int error = SYSCTL_IN(req, &addr, sizeof(addr));
    
    if (!error && req->newptr && req->newlen >= sizeof(struct array)) {
        struct array r;
        
        error = copyin(req->newptr, &r, sizeof(struct array));
        
        if(error == 0 && r.size > 0) {
            /* We have a new value stored in the standard location.*/
            /* Do with it as you see fit here. */
            DPRINT("sysctl %s: stored\n", oidp->oid_name);
            
            DPRINT("string %p size %d", r.values, r.size);
            
            freeUserInterfaces();
            
            userInterfaces = OSMalloc(sizeof(userInterfaces), globalOSMallocTag);
            if(userInterfaces == NULL) {
                DPRINT("unable to allocate userInterfaces\n");
                return -1;
            }
            
            char **ns = OSMalloc(r.size * sizeof(void**), globalOSMallocTag);
            if(ns == NULL) {
                OSFree(userInterfaces, sizeof(userInterfaces), globalOSMallocTag);
                userInterfaces = NULL;
                DPRINT("Unable to allocate memory for ns\n");
                return -1;
            }
            int count = 0;
            
            userInterfaces->size = r.size;
            userInterfaces->values = OSMalloc(userInterfaces->size * sizeof(userInterfaces->values), globalOSMallocTag);
            if(userInterfaces->values == NULL) {
                OSFree(ns, r.size * sizeof(void**), globalOSMallocTag);
                OSFree(userInterfaces, sizeof(userInterfaces), globalOSMallocTag);
                userInterfaces = NULL;
                DPRINT("unable to allocate userInterfaces->values\n");
                return -1;
            }
            
            error = copyin((user_addr_t) r.values, ns, r.size*sizeof(void**));
            if(error == 0) {
                DPRINT("string names %p %p", ns, ns[0]);
                
                for(int i = 0; i < r.size; i++) {
                    char *n = OSMalloc(MAXCOMLEN + 1, globalOSMallocTag);
                    if(n != NULL) {
                        size_t copied;
                        error = copyinstr((user_addr_t) ns[i], n, MAXCOMLEN, &copied);
                        
                        if(error == 0) {
                            DPRINT("string copied %zu bytes", copied);
                            DPRINT("string value = %s\n", n);
                            ifnet_t p_ifnet;
                            errno_t err = ifnet_find_by_name(n, &p_ifnet);
                            if(err) {
                                DPRINT("%s is not a valid interface\n", n);
                            }
                            userInterfaces->values[count++] = n;
                        } else if(error == ENAMETOOLONG) {
                            DPRINT("string input was too long!");
                        }
                    } else {
                        DPRINT("unable to allocate memory for a name for userinterfaces; skipping\n");
                    }
                }
            }
            
            if(ns) OSFree(ns, r.size * sizeof(void**), globalOSMallocTag);
        }
    } else if (req->newptr) {
        /* Something was wrong with the write request */
        /* Do something here if you feel like it.... */
        DPRINT("sysctl %s: write/error\n", oidp->oid_name);
    } else {
        /* Read request. Always return 763, just for grins. */
        DPRINT("sysctl interface %s: read\n", oidp->oid_name);
        DPRINT("old pts %llu old len %zu", req->oldptr, req->oldlen);
        if(req->oldptr && req->oldlen > sizeof(int)) {
            // has a pointer so put data structure in it
            struct array rar;
            
            error = copyin(req->oldptr, &rar, sizeof(struct array));
            
            DPRINT("rar %p\n", &rar);
            
            // TODO: better checks here for the correct size
            
            if(interfaces) {
                
                if(req->oldlen < sizeof(ifnet_t)) {
                    return SYSCTL_OUT(req, NULL, 0);
                }
                
                copyout(&ifCount, (user_addr_t) rar.size, sizeof(ifCount));
                
                char **ns = OSMalloc(rar.size * sizeof(void**), globalOSMallocTag);
                error = copyin((user_addr_t) rar.values, ns, rar.size*sizeof(void**));
                
                for(int i = 0; i < userInterfaces->size; i++) {
                    const char *name = ifnet_name(interfaces[i]);
                    size_t wrote = 0;
                    DPRINT("%d: %p\n", i, ns[i]);
                    copyoutstr(name, (user_addr_t) ns[i], strnlen(name, MAXCOMLEN), &wrote);
                }
                
                OSFree(ns, rar.size * sizeof(void**), globalOSMallocTag);
                
            } else if(userInterfaces) {
                
                if(req->oldlen < sizeof(struct array) + userInterfaces->size * sizeof(void**)) {
                    return SYSCTL_OUT(req, NULL, 0);
                }
                
                copyout(&(userInterfaces->size), (user_addr_t) rar.size, sizeof(userInterfaces->size));
                
                char **ns = OSMalloc(rar.size * sizeof(void**), globalOSMallocTag);
                error = copyin((user_addr_t) rar.values, ns, rar.size*sizeof(void**));
                
                for(int i = 0; i < userInterfaces->size; i++) {
                    size_t wrote = 0;
                    DPRINT("%d: %p\n", i, ns[i]);
                    copyoutstr(userInterfaces->values[i], (user_addr_t) ns[i], strnlen(userInterfaces->values[i], MAXCOMLEN), &wrote);
                }
                
                OSFree(ns, rar.size * sizeof(void**), globalOSMallocTag);
            } else {
                error = SYSCTL_OUT(req, NULL, 0);
            }
        } else if(req->oldptr && req->oldlen == sizeof(int)){
            // no pointer so just return the size
            DPRINT("nah inside here 2\n");
            
            int val = 0;
            if(interfaces) {
                val = ifCount;
            } else if (userInterfaces) {
                val = userInterfaces->size;
            }
            SYSCTL_OUT(req, &val, sizeof(int));
        }
        
        
        
    }
    return error;
}

int freeUserInterfaces() {
    if(userInterfaces != NULL) {
        if(userInterfaces->size > 0) {
            for(int i = 0; i < userInterfaces->size; i++) {
                if(userInterfaces->values[i]) OSFree(userInterfaces->values[i], MAXCOMLEN+1, globalOSMallocTag);
            }
            OSFree(userInterfaces->values, sizeof(userInterfaces->values) * userInterfaces->size, globalOSMallocTag);
        }
        
        userInterfaces->size = 0;
        
        OSFree(userInterfaces, sizeof(userInterfaces), globalOSMallocTag);
        
        userInterfaces = NULL;
    }
    
    return 0;
}


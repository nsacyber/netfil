/**
 * This is the main file for com.company.netfil
 * The sysctls are defined here as with each filters' setup and cleanup functions
 **/

#include "interface_filter.h"
#include "socket_filter.h"
#include "ip_filter.h"

kern_return_t iffilter_start(kmod_info_t * ki, void *d);
kern_return_t iffilter_stop(kmod_info_t *ki, void *d);

/**
 * this is the globlal OS Malloc tag for this application
 * all uses for OSMalloc/Free will use this tag
 **/
OSMallocTag globalOSMallocTag = NULL;

// Boundary crossing, sysctl
// https://developer.apple.com/library/content/documentation/Darwin/Conceptual/KernelProgramming/boundaries/boundaries.html#//apple_ref/doc/uid/TP30000905-CH217-BABJJBHG

// setup the root net.netfil sysctl
SYSCTL_NODE(_net, // our parent
            OID_AUTO , // automatically assign us an object ID
            netfil , // our name
            CTLFLAG_RW, // we wil be creating children therefore , read/write
            0, // Handler function ( none selected )
            BNAME
            );

//  _       _             __
// (_)     | |           / _|
//  _ _ __ | |_ ___ _ __| |_ __ _  ___ ___
// | | '_ \| __/ _ \ '__|  _/ _` |/ __/ _ \
// | | | | | ||  __/ |  | || (_| | (_|  __/
// |_|_| |_|\__\___|_|  |_| \__,_|\___\___|

SYSCTL_NODE(_net_netfil, // our parent
            OID_AUTO , // automatically assign us an object ID
            interface , // our name
            CTLFLAG_RW, // we wil be creating children therefore , read/write
            0, // Handler function ( none selected )
            BNAME
            );

SYSCTL_PROC(_net_netfil_interface, //our parent
            OID_AUTO, // automaticall assign us an object ID
            list, //our name
            CTLTYPE_STRUCT | CTLFLAG_RW | CTLFLAG_ANYBODY, //access flag ( read/write by anybody )
            NULL, // location of our data
            0, //argument passed to our handler
            sysctl_iff_interfaces_handler, // our handler function
            "S,array", // our date type
            "interfaces to use"); // our description

int interface_status = 0;
SYSCTL_PROC(_net_netfil_interface, //our parent
            OID_AUTO, // automaticall assign us an object ID
            status, //our name
            CTLFLAG_RW | CTLFLAG_ANYBODY | CTLTYPE_INT, //access flag ( read/write by anybody )
            &interface_status, // location of our data
            0, //argument passed to our handler
            sysctl_iff_status_handler, // our handler function
            "I", // our date type
            "interface filter status"); // our description

int user_interface_io_byte_limit = 0;
SYSCTL_INT(_net_netfil_interface,
           OID_AUTO,
           iobyte,
           CTLTYPE_INT | CTLFLAG_RW | CTLFLAG_ANYBODY,
           &user_interface_io_byte_limit,
           0,
           "Network interface i/o byte limit")

int user_interface_in_byte_limit = 0;
SYSCTL_INT(_net_netfil_interface,
           OID_AUTO,
           ibyte,
           CTLTYPE_INT | CTLFLAG_RW | CTLFLAG_ANYBODY,
           &user_interface_in_byte_limit,
           0,
           "Network interface in byte limit");

int user_interface_out_byte_limit = 0;
SYSCTL_INT(_net_netfil_interface,
           OID_AUTO,
           obyte,
           CTLTYPE_INT | CTLFLAG_RW | CTLFLAG_ANYBODY,
           &user_interface_out_byte_limit,
           0,
           "Network interface out byte limit");


//                 _        _
//                | |      | |
//  ___  ___   ___| | _____| |_
// / __|/ _ \ / __| |/ / _ \ __|
// \__ \ (_) | (__|   <  __/ |_
// |___/\___/ \___|_|\_\___|\__|


SYSCTL_NODE(_net_netfil, // our parent
            OID_AUTO , // automatically assign us an object ID
            socket , // our name
            CTLFLAG_RW, // we wil be creating children therefore , read/write
            0, // Handler function ( none selected )
            BNAME
            );

int socket_status = 0;
SYSCTL_PROC(_net_netfil_socket, //our parent
            OID_AUTO, // automaticall assign us an object ID
            status, //our name
            CTLFLAG_RW | CTLFLAG_ANYBODY | CTLTYPE_INT, //access flag ( read/write by anybody )
            &socket_status, // location of our data
            0, //argument passed to our handler
            sysctl_socket_status_handler, // our handler function
            "I", // our date type
            "socket filter status"); // our description

int user_pid = 0;
SYSCTL_INT(_net_netfil_socket,
           OID_AUTO,
           pid,
           CTLTYPE_INT | CTLFLAG_RW | CTLFLAG_ANYBODY,
           &user_pid,
           0,
           "Process ID");

int user_socket_io_byte_limit = 0;
SYSCTL_INT(_net_netfil_socket,
           OID_AUTO,
           iobyte,
           CTLTYPE_INT | CTLFLAG_RW | CTLFLAG_ANYBODY,
           &user_socket_io_byte_limit,
           0,
           "Network socket i/o byte limit")

int user_socket_in_byte_limit = 0;
SYSCTL_INT(_net_netfil_socket,
           OID_AUTO,
           ibyte,
           CTLTYPE_INT | CTLFLAG_RW | CTLFLAG_ANYBODY,
           &user_socket_in_byte_limit,
           0,
           "Network socket in byte limit");

int user_socket_out_byte_limit = 0;
SYSCTL_INT(_net_netfil_socket,
           OID_AUTO,
           obyte,
           CTLTYPE_INT | CTLFLAG_RW | CTLFLAG_ANYBODY,
           &user_socket_out_byte_limit,
           0,
           "Network socket out byte limit");

//  _
// (_)
//  _ _ __
// | | '_ \
// | | |_) |
// |_| .__/
//   | |
//   |_|

SYSCTL_NODE(_net_netfil, // our parent
            OID_AUTO , // automatically assign us an object ID
            ip, // our name
            CTLFLAG_RW, // we wil be creating children therefore , read/write
            0, // Handler function ( none selected )
            BNAME
            );

int ip_status = 0;
SYSCTL_PROC(_net_netfil_ip, //our parent
            OID_AUTO, // automaticall assign us an object ID
            status, //our name
            CTLFLAG_RW | CTLFLAG_ANYBODY | CTLTYPE_INT, //access flag ( read/write by anybody )
            &ip_status, // location of our data
            0, //argument passed to our handler
            sysctl_ip_status_handler, // our handler function
            "I", // our date type
            "Network IP filter status"); // our description

SYSCTL_PROC(_net_netfil_ip, //our parent
            OID_AUTO, // automaticall assign us an object ID
            addr, //our name
            CTLTYPE_STRUCT | CTLFLAG_RW | CTLFLAG_ANYBODY, //access flag ( read/write by anybody )
            NULL, // location of our data
            0, //argument passed to our handler
            sysctl_ip_addr_handler, // our handler function
            "S,array", // our date type
            "interfaces to use"); // our description

int user_ip_io_byte_limit = 0;
SYSCTL_INT(_net_netfil_ip,
           OID_AUTO,
           iobyte,
           CTLTYPE_INT | CTLFLAG_RW | CTLFLAG_ANYBODY,
           &user_ip_io_byte_limit,
           0,
           "Network IP i/o byte limit")

int user_ip_in_byte_limit = 0;
SYSCTL_INT(_net_netfil_ip,
           OID_AUTO,
           ibyte,
           CTLTYPE_INT | CTLFLAG_RW | CTLFLAG_ANYBODY,
           &user_ip_in_byte_limit,
           0,
           "Network IP in byte limit");

int user_ip_out_byte_limit = 0;
SYSCTL_INT(_net_netfil_ip,
           OID_AUTO,
           obyte,
           CTLTYPE_INT | CTLFLAG_RW | CTLFLAG_ANYBODY,
           &user_ip_out_byte_limit,
           0,
           "Network socket out byte limit");

/**
 * Unregisters all sysctls and run each filters' cleanup functions
 *
 * @returns 0 on success, error otherwise
 **/
kern_return_t cleanup() {
    kern_return_t ret = KERN_SUCCESS;
    
    sysctl_unregister_oid(&sysctl__net_netfil);
    
    sysctl_unregister_oid(&sysctl__net_netfil_interface);
    sysctl_unregister_oid(&sysctl__net_netfil_interface_list);
    sysctl_unregister_oid(&sysctl__net_netfil_interface_iobyte);
    sysctl_unregister_oid(&sysctl__net_netfil_interface_ibyte);
    sysctl_unregister_oid(&sysctl__net_netfil_interface_obyte);
    sysctl_unregister_oid(&sysctl__net_netfil_interface_status);
    
    sysctl_unregister_oid(&sysctl__net_netfil_socket);
    sysctl_unregister_oid(&sysctl__net_netfil_socket_pid);
    sysctl_unregister_oid(&sysctl__net_netfil_socket_iobyte);
    sysctl_unregister_oid(&sysctl__net_netfil_socket_ibyte);
    sysctl_unregister_oid(&sysctl__net_netfil_socket_obyte);
    sysctl_unregister_oid(&sysctl__net_netfil_socket_status);
    
    sysctl_unregister_oid(&sysctl__net_netfil_ip);
    sysctl_unregister_oid(&sysctl__net_netfil_ip_iobyte);
    sysctl_unregister_oid(&sysctl__net_netfil_ip_ibyte);
    sysctl_unregister_oid(&sysctl__net_netfil_ip_obyte);
    sysctl_unregister_oid(&sysctl__net_netfil_ip_addr);
    sysctl_unregister_oid(&sysctl__net_netfil_ip_status);
    
    if(interface_filter_cleanup() != 0) {
        DPRINT("Failed to cleanup interface filter\n");
        ret = KERN_FAILURE;
    }
    
    if(socket_filter_cleanup() != 0) {
        DPRINT("Failed to cleanup socket filter\n");
        ret = KERN_FAILURE;
    }
    
    if(ip_filter_cleanup() != 0) {
        DPRINT("Failed to cleanup ip filter\n");
        ret = KERN_FAILURE;
    }
    
    if(globalOSMallocTag) {
        OSMalloc_Tagfree(globalOSMallocTag);
    }
    
    DPRINT("iffilter_stop %d\n", ret);
    return KERN_SUCCESS;
    
}

/**
 * starts the kext
 * register SYSCTL and run each filters' setup function
 *
 * @param ki module info
 * @param d info
 * @returns 0 on success, error otherwise
 **/
kern_return_t iffilter_start(kmod_info_t * ki, void *d)
{
    kern_return_t ret = KERN_SUCCESS;
    
    if(!globalOSMallocTag) {
        globalOSMallocTag = OSMalloc_Tagalloc(BID, OSMT_DEFAULT);
    }
    
    if(!globalOSMallocTag) {
        ret = KERN_FAILURE;
        DPRINT("Unable to allocate tag\n");
        return ret;
    }
    
    // register sysctl oids
    sysctl_register_oid(&sysctl__net_netfil);
    
    sysctl_register_oid(&sysctl__net_netfil_interface);
    sysctl_register_oid(&sysctl__net_netfil_interface_list);
    sysctl_register_oid(&sysctl__net_netfil_interface_iobyte);
    sysctl_register_oid(&sysctl__net_netfil_interface_ibyte);
    sysctl_register_oid(&sysctl__net_netfil_interface_obyte);
    sysctl_register_oid(&sysctl__net_netfil_interface_status);
    
    sysctl_register_oid(&sysctl__net_netfil_socket);
    sysctl_register_oid(&sysctl__net_netfil_socket_pid);
    sysctl_register_oid(&sysctl__net_netfil_socket_iobyte);
    sysctl_register_oid(&sysctl__net_netfil_socket_ibyte);
    sysctl_register_oid(&sysctl__net_netfil_socket_obyte);
    sysctl_register_oid(&sysctl__net_netfil_socket_status);
    
    sysctl_register_oid(&sysctl__net_netfil_ip);
    sysctl_register_oid(&sysctl__net_netfil_ip_iobyte);
    sysctl_register_oid(&sysctl__net_netfil_ip_ibyte);
    sysctl_register_oid(&sysctl__net_netfil_ip_obyte);
    sysctl_register_oid(&sysctl__net_netfil_ip_addr);
    sysctl_register_oid(&sysctl__net_netfil_ip_status);
    
    // setup the interface filter
    if(interface_filter_setup() != 0) {
        DPRINT("Failed to setup interface filter\n");
        ret |= KERN_FAILURE;
    }
    
    // setup socket filter
    if(socket_filter_setup() != 0) {
        DPRINT("Failed to setup socket filter\n");
        ret |= KERN_FAILURE;
    }
    
    // setup socket filter
    if(ip_filter_setup() != 0) {
        DPRINT("Failed to setup ip filter\n");
        ret |= KERN_FAILURE;
    }
    
    if(ret != KERN_SUCCESS) {
        cleanup();
    }
    
    DPRINT("iffilter_start %d\n", ret);
    return ret;
}

/**
 * stops the kext and cleanups
 *
 * @param ki module info
 * @param d info
 * @returns 0 on success, error otherwise
 **/
kern_return_t iffilter_stop(kmod_info_t *ki, void *d)
{
    kern_return_t ret = KERN_SUCCESS;
    
    ret = cleanup();
    
    DPRINT("iffilter_stop %d\n", ret);
    return KERN_SUCCESS;
}

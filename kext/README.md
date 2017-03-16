To load the kext, use the `reload.sh` file in the root directoy of this workspace. This will unload the kext if present, give the new kext the proper permissions, then load it. 

Interface Filter Single interface example
     
	// start a fitler

     struct iff_filter filter;
     
     filter.iff_cookie   = 0;
     filter.iff_name     = "iffilter";
     filter.iff_output   = out_handler;
     filter.iff_input    = in_handler;
     filter.iff_protocol = 0;
     filter.iff_event    = event_handler;
     filter.iff_ioctl    = ioctl_handler;
     filter.iff_detached = NULL;
     
     errno_t err = ifnet_find_by_name("en4", &p_ifnet);
     if (err) {
     printf("interface en4 not found\n");
     return KERN_SUCCESS;
     }
     
     iflt_attach(p_ifnet, &filter, &filter_ref);

     // stop the filter

     if(p_ifnet != NULL) {
     ifnet_release(p_ifnet);
     }
     if(filter_ref != NULL) {
     iflt_detach(filter_ref);
     }

 The interface list is taken from userspace in an "array" structure

	 struct array {
	    int size;
	    void **values;
	};

the `size` is the number of null-terminated strings stored in values with a max length of `MAXCOMLEN`. See an example in the C example folder.

The ip addr sysctl takes an in_addr or an in6_addr. 
.section .sys.version, "ax"

.global lib_lwip_2_1_3_version

lib_lwip_2_1_3_version:

#ifdef CONFIG_RELEASE_ENABLE

#else

.asciz "LWIP_2_1_3-*modified*--@20250619-$95044bf3"
#endif

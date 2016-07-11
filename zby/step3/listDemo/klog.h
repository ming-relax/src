/*
 * klog.h -- Kernel debug function utils
 *
 * Copyright(c) 2011 NRCHPC, Inc. All rights reserved. 
 *
 * Author: Zhang Bingyin <zhangbingyin@nrchpc.ac.cn>
 *
 * Date: 2011/05/29
 *
 */

#ifndef _LINUX_KERNEL_DEBUG_H_
#define _LINUX_KERNEL_DEBUG_H_

//#define KLOG_ENABLED
#define KLOG_ENABLED_COMPACT

/* system diagnose key */
#define KLOG_TITLE		"1blkdrive"

/*
 * klog levels, used by user
 * they are mapped to different "kernel printk levels" according to BWCACHE_DEBUG
 */
#ifdef BWCACHE_DEBUG
/* use "dmesg -n 5" to prevent lower priorited msgs from other progs */
#define DEBUG			K_WARNING
#define INFO			K_ERR
#define WARN			K_CRIT
#define ERROR			K_ALERT
#define SERROR			K_ALERT
#define SFATAL			K_EMERG
#define DFATAL			K_EMERG	/* ONLY used when debugging */
#define FATAL			K_EMERG
#else
/* normal mapping when release */
#define DEBUG			K_DEBUG
#define INFO			K_INFO
#define WARN			K_NOTICE
#define ERROR			K_WARNING
#define SERROR			K_ERR
#define SFATAL			K_CRIT
#define DFATAL			K_ALERT
#define FATAL			K_EMERG
#endif

/*
 * translate following K_XXX to std KERN_XXX
 * e.g., K_ALERT(digit 0) to KERN_ALERT(string "<0>"
 */
#define U_TO_K_LEVEL(x)		"<" #x ">"

/* 
 * number of "kernel printk level", 
 * same to KERN_XXX except that they are digits 
 */
#define	K_EMERG			0
#define	K_ALERT			1
#define	K_CRIT			2
#define	K_ERR				3
#define	K_WARNING		4
#define	K_NOTICE			5
#define	K_INFO			6
#define	K_DEBUG			7

#ifdef KLOG_ENABLED
#define klog(n, f, a...)								\
	printk(U_TO_K_LEVEL(n) 								\
		KLOG_TITLE "_" #n ":(%s, %d, %s)\n\t" f,	\
		__FILE__, __LINE__, __FUNCTION__, ##a);
#else
#ifdef KLOG_ENABLED_COMPACT
#define klog(n, f, a...) printk(U_TO_K_LEVEL(n) f, ##a);
#else
#define klog(f, a...)
#endif
#endif

#endif /* _LINUX_KERNEL_DEBUG_H_ */

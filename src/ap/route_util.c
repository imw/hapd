/*
 * hostapd / route netlink api
 * Copyright (c) 2014, Isaac Wilder <imw@thefnf.org>
 *
 * This software may be distributed under GPL v3 or later
 */

#include "utils/includes.h"
#include <sys/ioctl.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/sockios.h>
#include <linux/if_vlan.h>

#include "utils/common.h"
#include "utils/eloop.h"
#include "hostapd.h"
#include "route_util.h"
#include "utils/os.h"

int rt_route(int del, int tid, char *gw)
{
	pid_t child;
	char tun[32] = {'\0'};
	char gwname[72] = {'\0'};
	char *table;
	table = os_zalloc(4);
	char tr[32] = {'\0'};

	strlcat(tun, "tunOut=gw-", 16);
	strlcat(tun, gw, sizeof(tun));
	strlcat(gwname, "/gwName=", 16);
	strlcat(gwname, gw, sizeof(gwname));
	strlcat(tr, "/tableRule=40000/", 18);
	snprintf(table, 4, "%d", tid);
	strlcat(tr, table, sizeof(tr));

	char *cmd[] = {"bmx6", "-c", tun, "/network=0.0.0.0/0", "/maxPrefixLen=0",
			 gwname, "/hysteresis=30", tr, NULL};
	if (del) {
		return 0;
                } else {
		child = fork();
		if( child >= 0 )
			if(child == 0)
				execvp("/usr/sbin/bmx6", cmd);
			else
				return 0;
		else
			return -1;
                }
}

int rt_mark(int del, int tid, char *src_if)
{
	int status;
	pid_t child;
	status = -1;
	char *act;

	act = os_zalloc(3);
	char *table;

	table = os_zalloc(4);
	snprintf(table, 4, "%d", tid);

	char physdev[32] = {'\0'};
	strlcat(physdev, src_if, 16);

	char *cmd[] = {"iptables", act, "PREROUTING", "-t", "mangle", "-m", "physdev", "--physdev-in",
				physdev, "-j", "MARK", "--set-mark", table, NULL};

	if (del) {
		strlcat(act, "-D", sizeof(act)+1);
        } else {
		strlcat(act, "-A", sizeof(act)+1);
        }

	child = fork();
	if (child >= 0)
		if( child == 0 )
			execvp("/usr/sbin/iptables", cmd);
		else
			return 0;
	else
		return -1;

}


int rt_rule(int del, int tid)
{
	int status;
	pid_t child;
	status = -1;
	char *command;
	command = os_zalloc(4);
	char *table;
	table = os_zalloc(4);
	snprintf(table, 4, "%d", tid);

	char *cmd[] = {"ip", "rule", command, "fwmark", table, "pref", "1",
				"table", table, NULL};

	if (del) {
		strlcat(command, "del", sizeof(command)+1);
        } else {
		strlcat(command, "add", sizeof(command)+1);
        }

	child = fork();
	if (child >= 0)
		if( child == 0 )
			execvp("/usr/sbin/ip", cmd);
		else
			return 0;
	else
		return -1;

}

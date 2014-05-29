#ifndef ROUTE_UTIL_H
#define ROUTE_UTIL_H

int rt_route(int del, int tid, char *gw);
int rt_mark(int del, int tid, char *src_if);
int rt_rule(int del, int tid);

#endif /* ROUTE_UTIL_H */

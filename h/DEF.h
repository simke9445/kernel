#ifndef DEF_H_
#define DEF_H_

//lock and unlock macros for pushing/poping PSW

#define lock() {asm{ pushf; cli; }}

#define unlock() {asm{ popf }}

		
#endif

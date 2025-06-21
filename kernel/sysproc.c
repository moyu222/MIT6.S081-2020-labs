#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// trace syscall
uint64
sys_trace(void)
{
  int mask;
  // 获得第一个参数即为 mask，用法参照上文
  if(argint(0, &mask) < 0)
    return -1;
  // 创建新的进程属性 tracenum，并赋值
  myproc()->tracenum = mask;
  return 0;
}

// sysinfo, fill out the fields of struct sysinfo
uint64
sys_sysinfo(void)
{
  struct sysinfo info;
  uint64 p;
  // 用于返回的指针由 user 在参数中提供
  if(argaddr(0, &p) < 0)
    return -1;

  // 分别在kallo.c pro.c 中实现这两个函数
  info.freemem = kfreemem_amt();
  info.nproc = getnpro_active();

  // 接下来在 user mode 和 kernel mode 之间将kernel中的结构体复制到user
  // 使用 copyout，参照 kernel/sysfile.c kernel/file.c
  if(copyout(myproc()->pagetable, p,(char*)&info, sizeof(info)) < 0)
    return -1;
  return 0;

}

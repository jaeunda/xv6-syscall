#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
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

int
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

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int
sys_hello_number(void){
  int n;
  if(argint(0, &n) < 0)
    return -1;
  // syscall.c:int argint(int n, int *ip); - Fetch the nth 32-bit system call argument
  cprintf("Hello, xv6! Your number is %d\n", n);
  return n*2;
}

struct k_procinfo {
	int pid, ppid, state; 
	uint sz; 
	char name[16];
}; // same as user space: struct procinfo
extern struct{
	struct spinlock lock;
	struct proc proc[NPROC];
} ptable;

int
sys_get_procinfo(void){
	// user.h: int get_procinfo(int pid, struct procinfo *uinfo);
	int pid;
	char *uaddr; // start address of user buffer
	struct proc *p, *t;
	struct k_procinfo kinfo;

	if (argint(0, &pid) < 0) return -1;
	if (argptr(1, &uaddr, sizeof(struct k_procinfo)) < 0) return -1;
	// syscall.c: int argptr(int n, char **pp, int size);
		// Fetch the nth word-sized system call argument as a pointer to a block of memory of size bytes.

	acquire(&ptable.lock);
	// spinlock.c: void acquire(struct spinlock *lk);
		// Acquire the lock. Loops(spins) until the lock is acquired.
	// proc.c: struct { struct spinlock lock; struct proc proc[NPROC]; } ptable;
		
	
	if (pid <= 0) t=myproc();
	// proc.c: struct proc *myproc();
		// return current process's state
		// struct cpu *c = mycpu(); struct proc *p = c->proc; return p;
		// struct cpu { ... struct proc *proc; // The process running on this cpu or null };
	else {
		t = (struct proc *)0; // NULL
		for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
			if (p->pid == pid) { t = p; break; }
	}
	if (t == (struct proc *)0 || (t->state == UNUSED))  { release(&ptable.lock); return -1; }

	kinfo.pid = (int)t->pid;
	kinfo.ppid = t->parent ? (int)t->parent->pid : (int)0; 
	kinfo.state = (int)t->state;
	kinfo.sz = (uint)t->sz; // size of memory (bytes)
	safestrcpy(kinfo.name, t->name, sizeof(kinfo.name));

	release(&ptable.lock);
	// spinlock.c: void release(struct spilock *lk);
		// Release the lock.

	if (copyout(myproc()->pgdir, (uint)uaddr, (void *)&kinfo, sizeof(kinfo)) < 0)
		return -1;
	return 0;

}

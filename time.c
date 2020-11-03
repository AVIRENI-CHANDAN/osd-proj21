#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

int main (int argc,char *argv[])
{

 int pid;
 int a=3,b=4;	
 pid = fork ();
 if (pid == 0)
 {	
    exec(argv[1],argv);
    //printf(1, "exec %s failed\n", argv[1]);
 }
  else
 {
    wait2(&a,&b);
 }  
 printf(1, "Wait Time = %d\n Run Time = %d with Status %d \n",a,b,wait2(&a,&b)); 
 exit();
}

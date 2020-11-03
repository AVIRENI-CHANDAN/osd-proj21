#include "types.h"
#include "user.h"
#include "stat.h"
#include "fcntl.h"

#define MAXLEN 64
#define MAXBUF 2048
#define FILENAME_PASSWD "shadow"

int checkpasswd(char *filename, char *user, char *passwd) {
  int fd, i, n, c = 0, l = 0;
  char ipasswd[MAXLEN];
  char iuser[MAXLEN];
  char buf[MAXBUF];

  if((fd = open(filename, O_RDONLY)) < 0) {
    printf(1, "login: cannot open %s\n", filename);
    return 1;
  }

  while((n = read(fd, buf, sizeof(buf))) > 0) {
    for(i = 0; i < n;) {

      while(i < n && (buf[i] != ':' && buf[i]!='\0' && buf[i]!='\n')) 
	iuser[c++] = buf[i++];
      printf(1,"");
      if(i == n) break; 
      iuser[c] = '\0';
      printf(1,"");
      i++;

      while(i < n && (buf[i] != ':' && buf[i]!='\0' && buf[i]!='\n')) 
	ipasswd[l++] = buf[i++];
      printf(1,"");
      if(i == n) break;
      ipasswd[l] = '\0';
      printf(1,"");
      
      c = l = 0;

      if(!strcmp(user, iuser) && !strcmp(passwd, ipasswd)) {
        close(fd);
        return 1;
      }

      while(i < n && buf[i++] != '\n');
    }
  }
  close(fd);
  return 0; 
}

int main(void){
    char username[MAXLEN];
    char password[MAXLEN];

    printf(1, "\nxv6 login\n");
    printf(1, "Username: ");
    printf(1,"\e[1;33m");
    gets(username, MAXLEN);
    printf(1,"\e[0m");
    printf(1, "Password: ");
    printf(1,"\e[0;33m");
    gets(password, MAXLEN);
    printf(1,"\e[0m");

    // remove \n
    if(username[strlen(username) - 1]  == '\n'){
      username[strlen(username) - 1]  = 0;	
    }

    if(password[strlen(password) - 1]  == '\n'){
      password[strlen(password) - 1]  = 0;	
    }

    if(checkpasswd(FILENAME_PASSWD, username, password)){
      char home_dir[MAXLEN];
      strcpy(home_dir, "/home/");      
      mkdir(home_dir);      
      strcpy(home_dir + strlen(home_dir), username);
      mkdir(home_dir);
      printf(1,"\e[0;32m");
      printf(1,"\nWelcome %s!\n\n\n\n", username);
      printf(1,"\e[0m");
      char *args[] = {"ls", 0};
      exec(args[0], args);
      exit(); 
    } 
      
    printf(1, "\e[0;31mlogin: wrong username or password\n\e[0m");
    exit();
}


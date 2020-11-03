#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "fs.h"

#define BUF_SIZE 256
#define MAX_LINE_NUMBER 256
#define MAX_LINE_LENGTH 256
#define NULL 0

char* strcat_n(char* dest, char* src, int len);
int get_line_number(char *text[]);
void show_text(char *text[]);
void com_ins(char *text[], int n, char *extra);
void com_mod(char *text[], int n, char *extra);
void com_del(char *text[], int n);
void com_help(char *text[]);
void com_save(char *text[], char *path);
void com_exit(char *text[], char *path);
int stringtonumber(char* src);

//Have the mark changed.
int changed = 0;
int auto_show = 1;

int main(int argc, char *argv[])
{
  //setProgramStatus(EDITOR);
  if (argc == 1)
  {
    printf(1, "please input the command as");
    printf(1,"\e[0;32m");
    printf(1," [editor file_name] \n\e[0m");
    //setProgramStatus(SHELL);
    exit();
  }
  //Store file content.
  
  char *text[MAX_LINE_NUMBER] = {};
  text[0] = malloc(MAX_LINE_LENGTH);
  memset(text[0], 0, MAX_LINE_LENGTH);
  //Store the current largest line number,starting from 0. That is, 
  //if line_nuber == x, from text.
  int line_number = 0;
  //Try to open the file.
  int fd = open(argv[1], O_RDONLY);
  //If file opens and read the contents in the file.
  if (fd != -1)
  {
    printf(1, "file");printf(1,"\e[0;32m");printf(1," exist\n");printf(1,"\e[0m");
    char buf[BUF_SIZE] = {};
    int len = 0;
    while ((len = read(fd, buf, BUF_SIZE)) > 0)
    {
      int i = 0;
      int next = 0;
      int is_full = 0;
      while (i < len)
      {
	//Copy the content before "\n".
        for (i = next; i < len && buf[i] != '\n'; i++)
          ;
        strcat_n(text[line_number], buf+next, i-next);
	//Create a new line if necessary.
        if (i < len && buf[i] == '\n')
        {
          if (line_number >= MAX_LINE_NUMBER - 1)
            is_full = 1;
          else
          {
            line_number++;
            text[line_number] = malloc(MAX_LINE_LENGTH);
            memset(text[line_number], 0, MAX_LINE_LENGTH);
          }
        }
        if (is_full == 1 || i >= len - 1)
          break;
        else
          next = i + 1;
      }
      if (is_full == 1)
        break;
    }
    close(fd);
  } else{
    printf(1,"File do not exist\n");
    exit();
  }
  
  //Output the file content
  show_text(text);
  //Show the output help
  com_help(text);
  
  //Processing commands
  char input[MAX_LINE_LENGTH] = {};
  while (1)
  {
    printf(1, "\nplease input command:\n");
    memset(input, 0, MAX_LINE_LENGTH);
    gets(input, MAX_LINE_LENGTH);
    int len = strlen(input);
    input[len-1] = '\0';
    len --;
    //Look for the first space.
    int pos = MAX_LINE_LENGTH - 1;
    int j = 0;
    for (; j < 8; j++)
    {
      if (input[j] == ' ')
      {
        pos = j + 1;
        break;
      }
    }
    //ins
    if (input[0] == 'i' && input[1] == 'n' && input[2] == 's')
    {
      if (input[3] == '-'&&stringtonumber(&input[4])>=0)
      {
        com_ins(text, stringtonumber(&input[4]), &input[pos]);
        //Insert operation need to update the row number.
        line_number = get_line_number(text);
      }
      else if(input[3] == ' '||input[3] == '\0')
      {
        com_ins(text, line_number+1, &input[pos]);
                                line_number = get_line_number(text);
      }
      else
      {
        printf(1, "invalid command.\n");
        com_help(text);
      }
    }
    //mod
    else if (input[0] == 'm' && input[1] == 'o' && input[2] == 'd')
    {
      if (input[3] == '-'&&stringtonumber(&input[4])>=0)
        com_mod(text, atoi(&input[4]), &input[pos]);
      else if(input[3] == ' '||input[3] == '\0')
        com_mod(text, line_number + 1, &input[pos]);
      else
      {
        printf(1, "invalid command.\n");
        com_help(text);
      }
    }
    //del
    else if (input[0] == 'd' && input[1] == 'e' && input[2] == 'l')
    {
      if (input[3] == '-'&&stringtonumber(&input[4])>=0)
      {
        com_del(text, atoi(&input[4]));
        //Delete operation need to update the row number.
        line_number = get_line_number(text);
      }  
      else if(input[3]=='\0')
      {
        com_del(text, line_number + 1);
        line_number = get_line_number(text);
      }
      else
      {
        printf(1, "invalid command.\n");
        com_help(text);
      }
      
    }
    else if (strcmp(input, "show") == 0)
    {
auto_show = 1;
      printf(1, "enable show current contents after text changed.\n");
    }
    else if (strcmp(input, "hide") == 0)
    {
      auto_show = 0;
      printf(1, "disable show current contents after text changed.\n");
    }
    else if (strcmp(input, "help") == 0)
      com_help(text);
    else if (strcmp(input, "save") == 0 || strcmp(input, "CTRL+S\n") == 0)
      com_save(text, argv[1]);
    else if (strcmp(input, "exit") == 0)
      com_exit(text, argv[1]);
    else
    {
      printf(1, "invalid command.\n");
      com_help(text);
    }
  }
  //setProgramStatus(SHELL);
  
  exit();
}

//拼接src的前n个字符到dest
//Splice the first n characters from src to dest.
char* strcat_n(char* dest, char* src, int len)
{
  if (len <= 0)
    return dest;
  int pos = strlen(dest);
  if (len + pos >= MAX_LINE_LENGTH)
    return dest;
  int i = 0;
  for (; i < len; i++)
    dest[i+pos] = src[i];
  dest[len+pos] = '\0';
  return dest;
}

void show_text(char *text[])
{
  printf(1,"\e[1;33m****************************************\n\e[0m");
  printf(1, "the contents of the file are:\n");
  int j;
  for (j=0; text[j] != NULL; j++)
    printf(1, "%d%d%d:%s\n", (j+1)/100, ((j+1)%100)/10, (j+1)%10, text[j]);
}

//获取当前最大的行号，从0开始，即return x表示text[0]到text[x]可用
//Get the current largest line number,starting from 0,that is, 
//return x means text[0] to text[x] are available.
int get_line_number(char *text[])
{
  int i = 0;
  for (; i < MAX_LINE_NUMBER; i++)
    if (text[i] == NULL)
      return i - 1;
  return i - 1;
}

int stringtonumber(char* src)
{
  int number = 0; 
  int i=0;
  int pos = strlen(src);
  for(;i<pos;i++)
  {
    if(src[i]==' ') break;
    if(src[i]>57||src[i]<48) return -1;
    number=10*number+(src[i]-48);
  }
  return number;
}

//插入命令，n为用户输入的行号，从1开始
//extra:输入命令时接着的信息，代表待插入的文本
//Insert command, n is the line number entered by the user, starting from 1
//extra: The information that follows when the command is entered, 
//representing the text to be inserted.

void com_ins(char *text[], int n, char *extra)
{
  if (n < 0 || n > get_line_number(text) + 1)
  {
    printf(1, "invalid line number\n");
    return;
  }
  char input[MAX_LINE_LENGTH] = {};
  if (*extra == '\0')
  {
    printf(1, "please input content:\n");
    gets(input, MAX_LINE_LENGTH);
    input[strlen(input)-1] = '\0';
  }
  else
    strcpy(input, extra);
  int i = MAX_LINE_NUMBER - 1;
  for (; i > n; i--)
  {
    if (text[i-1] == NULL)
      continue;
    else if (text[i] == NULL && text[i-1] != NULL)
    {
      text[i] = malloc(MAX_LINE_LENGTH);
      memset(text[i], 0, MAX_LINE_LENGTH);
      strcpy(text[i], text[i-1]);
    }
    else if (text[i] != NULL && text[i-1] != NULL)
    {
      memset(text[i], 0, MAX_LINE_LENGTH);
      strcpy(text[i], text[i-1]);
    }
  }
  if (text[n] == NULL)
  {
    text[n] = malloc(MAX_LINE_LENGTH);
    if (text[n-1][0] == '\0')
    {
      memset(text[n], 0, MAX_LINE_LENGTH);
      strcpy(text[n-1], input);
      changed = 1;
      if (auto_show == 1)
        show_text(text);
      return;
    }
  }
  memset(text[n], 0, MAX_LINE_LENGTH);
  strcpy(text[n], input);
  changed = 1;
  if (auto_show == 1)
    show_text(text);
}

//修改命令，n为用户输入的行号，从1开始
//extra:输入命令时接着的信息，代表待修改成的文本
//Modify command, which is the line number entered by the user, starting from 1.
//extra: The following information when entering the command, representing the text to
//be modified.
void com_mod(char *text[], int n, char *extra)
{
  if (n <= 0 || n > get_line_number(text) + 1)
  {
    printf(1, "invalid line number\n");
    return;
  }
  char input[MAX_LINE_LENGTH] = {};
  if (*extra == '\0')
  {
    printf(1, "please input content:\n");
    gets(input, MAX_LINE_LENGTH);
    input[strlen(input)-1] = '\0';
  }
  else
    strcpy(input, extra);
  memset(text[n-1], 0, MAX_LINE_LENGTH);
  strcpy(text[n-1], input);
  changed = 1;
  if (auto_show == 1)
    show_text(text);
}

//删除命令，n为用户输入的行号，从1开始
//Delete command, n is the line number entered by the user, starting from 1.
void com_del(char *text[], int n)
{
  if (n <= 0 || n > get_line_number(text) + 1)
  {
    printf(1, "invalid line number\n");
    return;
  }
  memset(text[n-1], 0, MAX_LINE_LENGTH);
  int i = n - 1;
  for (; text[i+1] != NULL; i++)
  {
    strcpy(text[i], text[i+1]);
    memset(text[i+1], 0, MAX_LINE_LENGTH);
  }
  if (i != 0)
  {
    free(text[i]);
    text[i] = 0;
  }
  changed = 1;
  if (auto_show == 1)
    show_text(text);
}

void com_help(char *text[])
{
  printf(1, "\e[1;33m****************************************\n\e[0m");
  printf(1, "instructions for use:\n");
  printf(1, "\e[0;32m");
  printf(1, "ins-n, insert a line after line n\n");
  printf(1, "mod-n, modify line n\n");
  printf(1, "del-n, delete line n\n");
  printf(1, "ins, insert a line after the last line\n");
  printf(1, "mod, modify the last line\n");
  printf(1, "del, delete the last line\n");
  printf(1, "show, enable show current contents after executing a command.\n");
  printf(1, "hide, disable show current contents after executing a command.\n");
  printf(1, "save, save the file\n");
  printf(1, "exit, exit editor\n");
  printf(1, "\e[0m");
}
void com_save(char *text[], char *path)
{
  //删除旧有文件
  //Delete old files.
  unlink(path);
  //新建文件并打开
  //Create new file and open.
  int fd = open(path, O_WRONLY|O_CREATE);
  if (fd == -1)
  {
    printf(1, "save failed, file can't open:\n");
    //setProgramStatus(SHELL);
    exit();
  }
  if (text[0] == NULL)
  {
    close(fd);
    return;
  }
  //写数据
  //Write data.
  write(fd, text[0], strlen(text[0]));
  int i = 1;
  for (; text[i] != NULL; i++)
  {
    printf(fd, "\n");
    write(fd, text[i], strlen(text[i]));
  }
  close(fd);
  printf(1, "saved successfully\n");
  changed = 0;
  return;
}

void com_exit(char *text[], char *path)
{
  //询问是否保存
  //Ask whether to save.
  while (changed == 1)
  {
    printf(1, "save the file? y/n\n");
    char input[MAX_LINE_LENGTH] = {};
    gets(input, MAX_LINE_LENGTH);
    input[strlen(input)-1] = '\0';
    if (strcmp(input, "y") == 0)
      com_save(text, path);
    else if(strcmp(input, "n") == 0)
      break;
    else
    printf(2, "wrong answer?\n");
  }
  //释放内存
  //Release memory.
  int i = 0;
  for (; text[i] != NULL; i++)
  {
    free(text[i]);
    text[i] = 0;
  }
  //退出
  //exit
  //setProgramStatus(SHELL);
  exit();
}

extern void syscall_readFile(char *fileName, char* buffer);
extern void syscall_printString(char *str);
extern void syscall_readString(char  *str); 
extern void syscall_readSector(char *buffer, int sector);

int main(){ 
    char buffer[13312];
    syscall_readFile("messag", buffer); 
    syscall_printString(buffer);
    while(1);
    return 0;
}

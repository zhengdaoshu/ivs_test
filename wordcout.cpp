#include <stdio.h>
#include<string.h>>
#include<math.h>
int getwordCount(const char* srcstr, char c)
{
    int count = 0;
    int i = 0;
        
    if(srcstr==NULL)
        return 0;
    int n=strlen(srcstr);
    for(i=0;i<n;i++)
    {
        if(srcstr[i]== c || fabs(srcstr[i]-c) ==32)
        {
            count++;
        }
    }
    return count;
}

int main()
{
    char buf[5000] ={0};
    char c;
    gets(buf);
    scanf("%c",&c);
    int n = getwordCount(buf,c);
    printf("%d\n",n);
    return 0;
}

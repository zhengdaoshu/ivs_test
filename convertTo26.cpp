#include<stdio.h>
#include<string.h>
void convertTo26(const char *input, char *output) 
{
    int i=0;
    int n;
    int num = 0;
    if(input == NULL || output == NULL)
    {
        printf("ERROR\n");
        return; 
    }

    n= strlen(input);
    if(n>6 || n <1)
    {
    	printf("ERROR\n");
        return;
	}
    for(i=0;i<n;i++)
    {
        if(input[i] <'a' ||  input[i] > 'z')
        {
            printf("ERROR\n");
            return;
        }
    }
    for(i=0;i<n;i++)
    {
        num = num * 26 + (input[i] - 'a' + 1);
    }
    sprintf(output, "%d",num);
}

int main()
{
    char input[64] = {0};
    char output[64] = {0};
    gets(input);
    convertTo26(input, output);
    printf("%s\n",output);
    return 0;
}

#include <stdio.h>
#include <string.h>

void parse_io_report(FILE * file, char * StrLine)
{
	int ix;
	
	// to indicate the location of ':' in StrLine
	int loc_filename, loc_lineno, loc_colno, loc_sign, loc_op;	
	char *substr;	// to store the substring of StrLine
    
    // get the location of first ':'
    substr = strchr(StrLine, ':');
    loc_filename = substr - StrLine;
    fprintf(file, "\t\"");
    for(ix = 0; ix < loc_filename; ix ++)
    	fprintf(file, "%c", StrLine[ix]);
    fprintf(file, "\",\n");
    
    // get the location of second ':'
    substr = strchr(substr + 1, ':');
    loc_lineno = substr - StrLine;
    fprintf(file, "\t");
    for(ix = loc_filename + 1; ix < loc_lineno; ix ++)
    	fprintf(file, "%c", StrLine[ix]);
    fprintf(file, ",\n");
    
    // get the location of third ':'
    substr = strchr(substr + 1, ':');
    loc_colno = substr - StrLine;
    fprintf(file, "\t");
    for(ix = loc_lineno + 1; ix < loc_colno; ix ++)
    	fprintf(file, "%c", StrLine[ix]);
    fprintf(file, ",\n");
    
    // get the location of fourth ':'
    substr = strchr(substr + 1, ':');
    loc_sign = substr - StrLine;
    if(StrLine[loc_sign + 2] == 'u')
    	fprintf(file, "\t1,\n");
    else if(StrLine[loc_sign + 2] == 's')
    	fprintf(file, "\t2,\n");
    else	// left shift
    {	
    	fprintf(file, "\t2,\n4\n");
    	return;
    }
    
    // get the location of fifth ':'
    substr = strchr(substr + 1, ':');
    loc_op = substr - StrLine;
    for(ix = loc_op + 2; ix < 1024; ix ++)
    	if(StrLine[ix] == '+')
    	{	
    		fprintf(file, "\t1\n");
    		break;
    	}
    	else if(StrLine[ix] == '-')
    	{
    		fprintf(file, "\t2\n");
    		break;
    	}
    	else if(StrLine[ix] == '*')
    	{
    		fprintf(file, "\t3\n");
    		break;
    	}
    	else
    		continue;
    	
    return;    
}

int main()
{
	/* get the line number */
    FILE * fp = NULL; //文件指针。
    int c, lc=0; //c为文件当前字符，lc为上一个字符，供结尾判断用。
    int line = 0; //行数统计
    fp = fopen("make.out", "r");//以只读方式打开文件。
    while((c = fgetc(fp)) != EOF) //逐个读入字符直到文件结尾
    {
        if(c == '\n') line ++; //统计行数。
        lc = c; //保存上一字符。
    }
    fclose(fp); //关闭文件
    if(lc != '\n') line ++;//处理末行
    
    printf("文件共有%d行。\n", line);
    
    /* read the file line by line */
    int ix = 0;    
	char StrLine[1024];             //每行最大读取的字符数
	if((fp = fopen("make.out","r")) == NULL) //判断文件是否存在及可读
  	{ 
	 	printf("error!"); 
	  	return -1; 
  	} 
  	
  	// open the file, into which we want to write our io-report structure
  	FILE * file = fopen("ios.def", "w");
  	if(file == NULL)
  	{
  		printf("ios.def OPEN error!\n");
  		return -1;
  	}
  	
  
  	while (!feof(fp)) 
    { 
    	if(ix == line)
    		break;
    	
    	// read the line    	
     	fgets(StrLine,1024,fp);  //读取一行
      	fprintf(file, "/*%s*/\n", StrLine); //输出
      	
      	//{filename, lineno, columnno, sign, opcode},
      	fprintf(file, "{\n");      	
      	parse_io_report(file, StrLine);   	
      	
      	if(ix == line -1)
      		fprintf(file, "}");
      	else
      		fprintf(file, "},\n");
      	
      	// update the index
      	ix ++;
  	}
 	fclose(fp);                     //关闭文件
 	
 	printf("ios.def is written.\n");    
    return 0;
}

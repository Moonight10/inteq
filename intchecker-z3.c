#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "intchecker-z3.h"

struct intfns_file_intop_node * file_intop_head;
struct intfns_intop_node * intop_head;
unsigned int intop_counter;

// get the INTOP-INDEX of STR in INTOP-LIST
// if not exists, we will allocate a new node
unsigned int search_intop(char * str)
{
	struct intfns_intop_node * temp = intop_head;
	while(temp->intop_next != NULL)
	{		
		temp = temp->intop_next;
		
		if(strcmp(temp->intop_name, str) == 0)
			return temp->intop_index;	
	}
		
	// not exists, we allocate a new node
	struct intfns_intop_node * temp1 = (struct intfns_intop_node * )malloc(sizeof(struct intfns_intop_node));
	strcpy(temp1->intop_name, str);
	intop_counter ++;
	temp1->intop_index = intop_counter;
	temp1->status = 0;
	
	// insert this new node into INTOP-HEAD
	temp1->intop_next = intop_head->intop_next;
	intop_head->intop_next = temp1;
		
	// at last, return the index of this new node
	return intop_counter;
}

//read struct data from file
void read_node()
{
	//read from the file
	printf("begin to read from log_intfns_INTOP_paths.txt--->>> ");
	FILE *f_read = fopen("/tmp/log_intfns_INTOP_paths.txt","r");
	if(f_read == NULL)
	{
		printf("f_read open ERROR\n");
		return;
	}
	
	char strline[1024];
	unsigned int num;
	unsigned int ix;
	unsigned int intop_index;
	
	while(fgets(strline, 1024, f_read) != NULL)
	{
		// Here, 'strline' stores the FILE-NAME		
		// allocate one file-node, and copy the file-name		
		struct intfns_file_intop_node * temp = (struct intfns_file_intop_node * )malloc(sizeof(struct intfns_file_intop_node));
		strcpy(temp->file_name, strline);
		
		// get the NUM of INTOP
		fgets(strline, 1024, f_read);
		num = atoi(strline);
		
		for(ix = 0; ix < num; ix ++)
		{
			fgets(strline, 1024, f_read);
			
			// at first, search the INTOP-HEAD and check whether this INTOP is already in INTOP-list
			// if so, we get the INDEX of this INTOP, and insert one new NODE into the INTOP-LIST of TEMP
			// if not, we will allocate a new INTOP in INTOP-HEAD, 
			// and also insert new NODE into the INTOP-LIST of temp			
			intop_index = search_intop(strline);
			
			// allocate one new NODE into TEMP
			struct intfns_file_intop_node * temp1 = (struct intfns_file_intop_node * )malloc(sizeof(struct intfns_file_intop_node));
			temp1->intop_index = intop_index;
			temp1->file_next = NULL;
			temp1->intop_next = temp->intop_next;
			temp->intop_next = temp1;
		}
		
		// at last, insert this new TEMP into FILE-INTOP-HEAD
		temp->file_next = file_intop_head->file_next;
		file_intop_head->file_next = temp;
	}
	
	printf("read OK\n");
}

// search INTOP-NODE list, and delete the NODE whose intop-index is INDEX
void update_intop_status(unsigned int index)
{	
	struct intfns_intop_node * prev = intop_head;
	struct intfns_intop_node * temp;
	
	while(prev->intop_next != NULL)
	{		
		temp = prev->intop_next;
		
		// if found, detele this TEMP
		if(temp->intop_index == index)
		{	
			prev->intop_next = temp->intop_next;	
			free(temp);	// delete this TEMP			
		}
		else
			prev = temp;	// continue;
	}
}

// update the status of INTOP according to the z3 results
// if one path is SAT, we will update the STATUS of INTOP as 1
// at last, all the INTOP, whose status is 0, is unharmful
void update_node()
{
	//read z3 results from the file
	printf("begin to read from result.txt--->>> ");
	FILE *f_read = fopen("/tmp/solving_result.txt","r");
	if(f_read == NULL)
	{
		printf("f_read open ERROR\n");
		return;
	}
	
	char strline[1024];
	unsigned int intop_index;
	
	while(fgets(strline, 1024, f_read) != NULL)
	{
		// Here, 'strline' store the FILE-NAME		
		// search the FILE-INTOP-HEAD and locate the NODE with 'strline' as file-name
		unsigned int found = 0;
		struct intfns_file_intop_node * temp = file_intop_head;
		while(temp->file_next != NULL)
		{		
			temp = temp->file_next;
		
			if(strcmp(temp->file_name, strline) == 0)
			{
				found = 1;
				break;
			}	
		}
		
		//if not exist,read another line and continue
		if(found != 1)
		{
			fgets(strline, 1024, f_read);
			continue;
		}
		
		// Here, TEMP is the file-intop-node whose 'file-name' is 'strline'
		// get the z3 result. if the result is 'sat', we have to update the STATUS of all INTOP in TEMP
		fgets(strline, 1024, f_read);
		if(strncmp(strline, "sat", 3) == 0)
		// if(strncmp(strline, "unsat", 5) != 0)
		{
			struct intfns_file_intop_node * temp1 = temp;
			while(temp1->intop_next != NULL)
			{
				temp1 = temp1->intop_next;
				// search all INTOP-list, and delete the node whose INTOP-INDEX is temp1->intop_index
				update_intop_status(temp1->intop_index);
			}
		}
		
		// fixme: after analysis, we should delete TEMP and all its INTOP out from FILE-INTOP-HEAD list
		// so as to gain better analysis efficiency		
	}
	
	printf("read OK\n");
}


// dump all unharmful intops
void dump_unharmful_intop_node()
{
	FILE * file = fopen("/tmp/intfns_unharmful_intops.txt", "w");
	
	struct intfns_intop_node * temp = intop_head;
	
	while(temp->intop_next != NULL)
	{		
		temp = temp->intop_next;
		fprintf(file, "%s", temp->intop_name);	
	}
	
	fclose(file);
	return;
}


//free the list
void free_node()
{
	// delete all INTOP-NODE list
	while(intop_head)
	{
		struct intfns_intop_node * temp;
		temp = intop_head;
		intop_head = intop_head->intop_next;
		free(temp);
	}
	
	// delete all FILE-INTOP-NODE list
	while(file_intop_head)
	{
		struct intfns_file_intop_node * temp;
		temp = file_intop_head;
		file_intop_head = file_intop_head->file_next;
		
		// delete all INTOP-NODE of TEMP
		while(temp)
		{
			struct intfns_file_intop_node * temp1;
			temp1 = temp;
			temp = temp->intop_next;
			free(temp1);
		}
	}
}


void main()
{	
	// 0. init the global vars
	file_intop_head = (struct intfns_file_intop_node * )malloc(sizeof(struct intfns_file_intop_node));
	file_intop_head->intop_index = 0;
	file_intop_head->file_next = NULL;
	file_intop_head->intop_next = NULL;
	
	intop_head = (struct intfns_intop_node * )malloc(sizeof(struct intfns_intop_node));
	intop_head->intop_index = 0;
	intop_head->intop_next = NULL;
	
	intop_counter = 0;

	//1. read data from the files
	read_node();
	
	//2. update the status of INTOP
	update_node();
	
	//3. dump unharmful intop
	dump_unharmful_intop_node();
	
	//4. at last, release the space
	free_node();
}



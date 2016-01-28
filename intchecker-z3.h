//struct for FILE-INTOP-LIST
struct intfns_file_intop_node{
	char file_name[1024];
	unsigned int intop_index;	
	struct intfns_file_intop_node * file_next;
	struct intfns_file_intop_node * intop_next;
};

//struct for INTOP-LIST
struct intfns_intop_node{
	char intop_name[1024];
	unsigned int intop_index;
	unsigned int status;
	struct intfns_intop_node * intop_next;
};

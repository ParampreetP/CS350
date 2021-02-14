// ADD YOUR IMPLEMENTATION IN THIS FILE
#include "my_mem_allocator.h"
BLOCK_HDR * fl_head;
void my_free(void *ptr)
{  
	BLOCK_HDR * fl_node = ptr-0x10;
	if((long)(fl_node -> next) != (long)MAGIC_NUM){
    		//DEBUG_MSG("my_free error: Double Free or Header Corrupton at %p.\n", ptr);
		return;
	}
	//ptr -=16;
	if(fl_head == NULL){
		fl_node ->next = NULL; 
		fl_head = fl_node;
		return;
	}
	if(fl_head>=fl_node){
		fl_node->next = fl_head;
		fl_head = fl_node;
	}
	else if(fl_head->next == NULL && fl_head < fl_node){
		fl_node->next = NULL;
		fl_head->next = fl_node;
	}
	else{
		BLOCK_HDR * prev = fl_head;
		while(prev->next != NULL && prev->next < ptr-16){
			prev = prev->next;
		}

		if(prev->next == NULL){
			fl_node->next = NULL;
			prev->next = fl_node;
		}
		else if(prev->next >=(void *) fl_node){
			fl_node->next = prev->next;
			prev->next = fl_node;
		}
		else{
			//DEBUG_MSG("Error at %p.\n", ptr);
			return;
		}
	}
	BLOCK_HDR * fl_combine = fl_head;
	while(fl_combine != NULL){
		while((void *)fl_combine + sizeof(BLOCK_HDR) + fl_combine->size == fl_combine->next){
			BLOCK_HDR * fl_found = (BLOCK_HDR *)fl_combine->next;
			fl_combine->size = fl_combine->size + sizeof(BLOCK_HDR) + fl_found->size;
			fl_combine->next = fl_found->next; 
		}
		fl_combine = fl_combine->next;
	}
	return;
}

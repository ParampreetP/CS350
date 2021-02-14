// ADD YOUR IMPLEMENTATION IN THIS FILE
#include "my_mem_allocator.h"
BLOCK_HDR * fl_head;
void addtofl(BLOCK_HDR * fl_node){
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
		while(prev->next != NULL && prev->next < (void *)(fl_node)){
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
			//DEBUG_MSG("Error at %p.\n", (void *) fl_node);
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


void *my_realloc(void *ptr, size_t reqsize)
{
	void * usr_ptr = ptr;
	BLOCK_HDR * alloc_block = (BLOCK_HDR *)(ptr-0x10);
	if((long)(alloc_block -> next) != (long)MAGIC_NUM){
    		//DEBUG_MSG("my_realloc error: INVALID OLD POINTER\n");
		return NULL;
	}
	if(reqsize < alloc_block->size){
		if(alloc_block->size - reqsize >= sizeof(BLOCK_HDR) +1 ){
			BLOCK_HDR * fl_node;
			fl_node = ptr + reqsize;  
			fl_node->size = alloc_block->size - reqsize -sizeof(BLOCK_HDR);
			alloc_block->size = reqsize;
			addtofl(fl_node);
			return usr_ptr;
		}
		else{
			return usr_ptr;
		}
	}
	else if (reqsize > alloc_block->size){
		BLOCK_HDR * chosen;
		BLOCK_HDR * prev = NULL;
		if (fl_head ->next == NULL && fl_head -> size < reqsize){
			//DEBUG_MSG("Failed to allocate memory of requested size %d.\n", reqsize);
			return NULL;
		}
		BLOCK_HDR * temp_block = fl_head;
		while(temp_block->next != NULL && temp_block->size < reqsize){
			prev = temp_block;			
			temp_block = temp_block->next;
		}
		if(temp_block->size >=reqsize){
			usr_ptr = temp_block+1;
			
			chosen = temp_block;
		}
		else{
			//DEBUG_MSG("Failed to allocate memory of requested size %d.\n", reqsize);
			return NULL;
		}
		if(chosen->size - reqsize >= 17){
			BLOCK_HDR * fl_node;
			fl_node = (void *)chosen + reqsize + sizeof(BLOCK_HDR);
			fl_node->size = chosen->size - reqsize - sizeof(BLOCK_HDR);
			if(prev == NULL && chosen->next == NULL){
				fl_node->next = NULL;
				fl_head = fl_node;
			}
			else if(chosen->next == NULL){
				fl_node->next = NULL;
				prev->next = fl_node;
			}
			else if(prev == NULL){
				fl_node ->next = chosen->next;
				fl_head = fl_node;
			}
			else{
				fl_node->next = chosen->next;
				prev->next = fl_node;
			}
			chosen->size = reqsize;
			chosen->next = (void *)MAGIC_NUM;
		}
		else{
			if(prev == NULL && chosen->next == NULL){
				fl_head = NULL;
			}
			else if(chosen->next == NULL){
				prev->next == NULL;
			}
			else if(prev == NULL){
				fl_head = chosen->next;
			}
			else{
				prev->next = chosen->next;
			}
			
			chosen->next = (void *)MAGIC_NUM;
		}
		memcpy((void *)(chosen +1), ptr, alloc_block->size);
		usr_ptr = chosen +1;
		addtofl(alloc_block);
		return usr_ptr;
	}
	else{
		return usr_ptr;
	}
}

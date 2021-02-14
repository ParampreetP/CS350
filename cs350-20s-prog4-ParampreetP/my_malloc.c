// ADD YOUR IMPLEMENTATION IN THIS FILE

// --- FOR STUDETS: "Segmentation fault" usually happens when deferencing a null pointer.
// --- So be careful with you code logic to avoid the fault from happening.
// --- If you need to debug a seg fault, keep the above mind so you can find out the problem quickly.            
#include "my_mem_allocator.h"

BLOCK_HDR *fl_head;
ALLOC_POLICY alloc_policy;
void *my_malloc(size_t reqsize)
{	

	BLOCK_HDR * chosen; 
	BLOCK_HDR *prev = NULL;
	void * usr_ptr = NULL;
	if(alloc_policy == AP_FIRST){
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
	}
	else{
		int best = 0x100000;
		if(fl_head->size >= reqsize){
			best = fl_head->size;
			chosen = fl_head;
		}
		usr_ptr = fl_head +1;
		BLOCK_HDR *temp_block = fl_head;
		while(temp_block ->next != NULL){
			BLOCK_HDR * check = (BLOCK_HDR *) temp_block->next;
			if(check-> size < reqsize){
				temp_block = temp_block->next;
			}
			else if(check->size < best){
				prev = temp_block;
				best = check->size;
				usr_ptr = temp_block->next +16;
				chosen = temp_block->next;
				temp_block = temp_block->next;				
			}
			else{
				temp_block = temp_block->next;
			}
		}
		if(best == 0x100000){
			//DEBUG_MSG("my_alloc error: No free block can accommodate requested size of: %d.\n", reqsize);
			return NULL;
		}
		
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
			prev->next == fl_node;
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
		//chosen->size = reqsize; MIGHT NEED THIS
		chosen->next = (void *)MAGIC_NUM;
	}
	//printf("%pCHECK"	,usr_ptr);
	return usr_ptr;
}

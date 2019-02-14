/**
 * Mini Valgrind Lab
 * CS 241 - Spring 2019
 */

 #include "mini_valgrind.h"
  #include <stdio.h>
  #include <string.h>

    size_t total_memory_requested = 0;
    size_t total_memory_freed=0;
    size_t invalid_addresses = 0;
    meta_data* head =NULL;


  void *mini_malloc(size_t request_size, const char *filename,
                    void *instruction) {
      if(request_size==0){
        return NULL;
      }
      meta_data * m1 = malloc(request_size+ sizeof(meta_data));
      m1->request_size = request_size;
      m1->filename = filename;
      m1->instruction = instruction;

      m1->next=head;
      head=m1;
      total_memory_requested = total_memory_requested + request_size;
      // your code here

      return head+1;
  }

  void *mini_calloc(size_t num_elements, size_t element_size,
                    const char *filename, void *instruction) {
      // your code here
      if(num_elements==0||element_size==0) return NULL;
      // calloc(a, b) === calloc(a*b, 1) === {malloc (a*b), memset()}
      meta_data * m1 = calloc((num_elements*element_size+sizeof(meta_data)),1);
      m1->request_size = num_elements*element_size;
      m1->filename = filename;
      m1->instruction = instruction;

      m1->next=head;
      head=m1;
      total_memory_requested = total_memory_requested + num_elements*element_size;

      return head+1;
  }

  void *mini_realloc(void *payload, size_t request_size, const char *filename,
                     void *instruction) {
      // your code here
      if(payload==NULL) return mini_malloc(request_size,filename,instruction);
      if(request_size==0) {
        mini_free(payload);
        return NULL;
      }


      meta_data* ptr = ((meta_data*)payload)-1;
      int origin = (int)ptr->request_size;
      //mini_free(ptr)
      //ptr = malloc()
      //find prev
      //int flagvalid = 0;
      meta_data * cur = head;
      meta_data * curprev = NULL;
      while (cur!=NULL) {
       // printf("Billy" );
       if(ptr==head){
         head = head->next;
       //  free(ptr);
         break;
       }
       else if(cur==ptr) {
         curprev->next = cur->next;
       //  free(ptr);

          break;
          }
       curprev = cur;
        cur = cur->next;
      }
      if(cur==NULL){
        invalid_addresses++;
       // printf("dog" );
        return NULL;
      }

     //  else if(flagvalid){
      // meta_data * prev = head;
      //(-1)
      // while(prev->next!=ptr){
      //   prev++;
      // }
      //
      // prev++;
      //
      // prev->next=ptr->next;
        int n = (int)request_size- origin;
       ptr = realloc(ptr, request_size+sizeof(meta_data));
      //
       //int n = (int)request_size- (int)ptr->request_size;
      //
       ptr->request_size = request_size;
       ptr -> filename = filename;
       ptr -> instruction = instruction;
       ptr -> next = head;
       head = ptr;
       if(n>0)
       total_memory_requested = total_memory_requested+ n;
       else
       total_memory_freed = total_memory_freed-n;
       return ptr+1;

   // }
  }

  void mini_free(void *payload) {
      // your code here
      if(payload==NULL) return;
      int flagvalid = 0;
      meta_data * mp = (meta_data*)payload-1;
      meta_data* cur = head;
      while (cur!=NULL) {
        //printf("Billy" );
        if(cur==mp) {
          flagvalid = 1;
          }
        cur = cur->next;
      }
      //printf("laoalll" );
      if(flagvalid ==0){
        invalid_addresses++;
        //printf("dog" );
      }
      else if(flagvalid){
        //printf("cat");
        total_memory_freed = total_memory_freed+(mp->request_size);
        // printf("%d\n", mp->request_size);
         //printf("%d\n", total_memory_freed);
        meta_data* prev = head;
        while (prev->next!=mp&&prev!=mp) {
          prev = prev->next;
        }
        prev->next = mp->next;
        mp->next=NULL;
        if(head==mp) head = mp->next;

          free(mp);
          mp=NULL;


      }
  }

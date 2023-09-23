#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

int main() {
    printf("Tests for linked list implementation\n");
    list_t* mylist = list_alloc();

    // test for add front
    list_add_to_front(mylist,1);
    list_add_to_front(mylist,3);
    //test for add back
    list_add_to_back(mylist,2);
    list_add_to_back(mylist,9);
    //test for some index
    list_add_at_index(mylist,4,1);
    list_add_at_index(mylist,5,2);
    // must be 3-4-5-1-2-9
    if(strcmp("3->4->5->1->2->9->NULL",listToString(mylist)) == 0)
    {
        printf("list_add_at_index : SUCCESSFUL\n");
    }
    printf(" print the linked list \n");
    list_print(mylist);
    printf("printing is ended \n ");
    printf("length of list is %d : ",list_length(mylist));
    printf("\n");
    printf("Remove\n");
    printf("%d\n",list_remove_from_back(mylist)); // 9
    printf("%d\n",list_remove_from_back(mylist)); // 2
    printf("%d\n",list_remove_from_front(mylist)); // 3
    printf("%d\n",list_length(mylist)); // 3
    // after 4-5-1
    printf("checking my list : ");
    list_print(mylist);
    printf("Check\n");
    printf("%s\n", list_is_in(mylist,5)?"True":"False");
    printf("%s\n", list_is_in(mylist,9)?"True":"False");
    list_add_to_back(mylist,6);
    printf("%d\n", list_get_elem_at(mylist,0));
    printf("%d\n", list_get_elem_at(mylist,90));
    list_print(mylist);
    printf("Index\n");
    printf("%d\n", list_get_index_of(mylist,2));
    printf("%d\n", list_get_index_of(mylist,3));
    printf("Remove at index\n");
    printf("%d\n",list_remove_at_index(mylist,0));
    return 0;

}

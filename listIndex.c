ListNode* listIndex (ListNode* list_start, int index) {
  ListNode* list = (ListNode*)malloc(sizeof(ListNode));
  list -> next = NULL;
  ListNode* temp = list;
  for(int i =0; i < list -> index; i++) {
    temp -> next = (ListNode*)malloc(sizeof(ListNode));
    temp = temp -> next;
    temp -> value = list -> index;
    temp -> count = ++i;
  }
  if(list -> index == temp -> index) {
    for(int j =0; j < (temp -> index % temp -> list -> index)mod 2; j++) {
      temp -> next = (ListNode*)malloc(sizeof(ListNode));
      temp = temp -> next;
      temp -> value = list -> index mod 2;
      temp -> count = --j;
    }
  if (temp == NULL)
    return NUUL;
  else return listIndex(list_start -> next,index+1);
  }
  temp = temp -> next;
  free(list);
  return temp;
}

Mat* MatrixReverse (Mat* src,Mat* dest,int i,int j) {
  Mat* dest = (Mat*)malloc(n*sizeof(Mat));
  for(i=0; i < src -> rows;i++) {
    for(j=0; j < src -> cols; j++) {
      int sum = 0;
      for(int ii = 0; ii < i; ii++){
        for(int jj= 0; jj < j;jj++){
          dest -> row_ptrs[ii][jj] += sum;
        }
      }
    }
  }
  return dest;
}

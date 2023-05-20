#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct tree_node
{
   int              ID;
   char             *name;
   struct tree_node *left;
   struct tree_node *right;
} node_t;


void print_bst(node_t *node)
{
   if(node == NULL) {printf("Tree is empty!\n"); return;}

   if (node != NULL) printf("%d %s: \t", node->ID, node->name);
   if (node->left != NULL) printf("L%d,", node->left->ID);
   if (node->right != NULL) printf("R%d", node->right->ID);
   printf("\n");

   if (node->left != NULL)
      print_bst(node->left);
   if (node->right != NULL)
      print_bst(node->right);
}


void delete_tree(node_t **node)
{
   if (*node != NULL) {

      // recursively delete
      delete_tree(&(*node)->left);
      delete_tree(&(*node)->right);
      
      // free memory
      free((*node)->name);
      free(*node);

      // reset pointer
      *node = NULL;
   }
}

void insert(node_t **node, int ID, char *name)
{
   // found place to insert
   if (*node == NULL) {
      *node = malloc(sizeof(node_t));
      (*node)->ID = ID;
      (*node)->name = strdup(name);
      (*node)->left = NULL;
      (*node)->right = NULL;
   
   // left sub tree
   } else if ((*node)->ID > ID)
      insert(&(*node)->left, ID, name);

   // right sub tree
   else
      insert(&(*node)->right, ID, name);
}


void search(node_t *node, int ID)
{
   // plant not found
   if (node == NULL)
      printf("Plant with ID %d does not exist!\n", ID);
   
   // left tree
   else if (node->ID > ID)
      search(node->left, ID);

   // right tree
   else if (node->ID < ID)
      search(node->right, ID);
   
   // plant found
   else
      printf("Plant with ID %d has name %s!\n", ID, node->name);
}


int main(int argc, char const *argv[])
{
   node_t *root = NULL;  // empty tree
   printf("Inserting nodes to the binary tree.\n");

   insert(&root, 445, "sequoia");

   insert(&root, 162, "fir");
   insert(&root, 612, "baobab");
   insert(&root, 845, "spruce");
   insert(&root, 862, "rose");
   insert(&root, 168, "banana");
   insert(&root, 225, "orchid");
   insert(&root, 582, "chamomile"); 

   printf("Printing nodes of the tree.\n");
   print_bst(root);


   search(root, 168);
   search(root, 467);

   printf("Deleting tree.\n");
   delete_tree(&root);

   print_bst(root);
   return 0;
}

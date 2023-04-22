#ifndef XUTREE
#define XUTREE

#include "../../AlgoritmosProject_2C_EMPTY_v2.3/src/biblioteca/tads/Stack.hpp"
using namespace std;


// Info de los nodos del arbol
struct HuffmanTreeInfo
{
   unsigned int c; // int correspondiente al char en ascii
   unsigned int n; //cant de ocurrencias
   HuffmanTreeInfo* left;
   HuffmanTreeInfo* right;
};

// ---------------------
// -- TAD HuffmanTree --

struct HuffmanTree
{
   Stack<HuffmanTreeInfo*> pila;
   Stack<string> pilaCod;
};

// init function
HuffmanTree huffmanTree(HuffmanTreeInfo* root)
{
   HuffmanTree tu;
   tu.pila = stack<HuffmanTreeInfo*>();
   tu.pilaCod = stack<string>();
   stackPush<HuffmanTreeInfo*>(tu.pila,root);
   stackPush<string>(tu.pilaCod,"");
   return tu;
}

bool huffmanTreeHasNext(HuffmanTree ht)
{
   return !stackIsEmpty<HuffmanTreeInfo*>(ht.pila);
}

HuffmanTreeInfo* huffmanTreeNext(HuffmanTree& tu,string& cod)
{
   bool hoja=false;
   HuffmanTreeInfo* p = NULL;
   HuffmanTreeInfo* aux = NULL;
   string zz;

   while( !stackIsEmpty<HuffmanTreeInfo*>(tu.pila) && !hoja )
   {
      p = stackPop<HuffmanTreeInfo*>(tu.pila);
      zz = stackPop<string>(tu.pilaCod);

      if( p->right!=NULL )
      {
         stackPush<HuffmanTreeInfo*>(tu.pila,p->right);
         aux = new HuffmanTreeInfo();
         aux->n = 1;
         stackPush<string>(tu.pilaCod,zz+"1");
      }

      if( p->left!=NULL )
      {
         stackPush<HuffmanTreeInfo*>(tu.pila,p->left);
         aux = new HuffmanTreeInfo();
         aux->n = 0;
         stackPush<string>(tu.pilaCod,zz+"0");
      }

      if( p->left==NULL && p->right==NULL)
      {
         hoja = true;
         cod.clear();
         cod.append(zz);
      }
      else
      {
         p = NULL;
      }
   }

   return p;
}

#endif

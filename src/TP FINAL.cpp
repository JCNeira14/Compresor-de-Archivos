//============================================================================
// Name        : TP.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#ifndef _MAIN
#define _MAIN

#include <iostream>
#include "../../AlgoritmosProject_2C_EMPTY_v2.3/src/biblioteca/tads/Arr.hpp"
#include "../../AlgoritmosProject_2C_EMPTY_v2.3/src/biblioteca/tads/Map.hpp"
#include "../../AlgoritmosProject_2C_EMPTY_v2.3/src/biblioteca/tads/List.hpp"
#include "../../AlgoritmosProject_2C_EMPTY_v2.3/src/biblioteca/tads/Stack.hpp"
#include "../../AlgoritmosProject_2C_EMPTY_v2.3/src/biblioteca/tads/Queue.hpp"
#include "../../AlgoritmosProject_2C_EMPTY_v2.3/src/biblioteca/funciones/strings.hpp"
#include "../../AlgoritmosProject_2C_EMPTY_v2.3/src/biblioteca/funciones/tokens.hpp"
#include "../../AlgoritmosProject_2C_EMPTY_v2.3/src/biblioteca/funciones/files.hpp"
#include "../../AlgoritmosProject_2C_EMPTY_v2.3/src/biblioteca/tads/Bits.hpp"
#include "TP FINAL.hpp"

using namespace std;

template<typename T>
T listRemoveFirst(List<T>& lst)
{
   return removeFirst<T>(lst.nodo);
}

struct HuffmaneTable
{
   unsigned int n; //contador de ocurrencias
   string code; //codigo sacado del arbol
};

void contarOcurrencias(string fName,HuffmaneTable tabla[])
{
   FILE* f=fopen(fName.c_str(),"r+b");
   unsigned char c=read<unsigned char>(f);
   while(!feof(f))
   {
      int ascii=c;
      tabla[ascii].n++;

      c=read<unsigned char>(f);
   }
   fclose(f);
}

int cmpHufHuf(HuffmanTreeInfo a,HuffmanTreeInfo b)
{
   int ret;
   if(a.n==b.n)
   {
      a.c<b.c?ret=-1:ret=1;
   }
   else
   {
      if(a.n<b.n)
      {
         ret=-1;
      }
      else
      {
         ret=1;
      }
   }
   return ret;
}

void crearLista(List<HuffmanTreeInfo>& lista,HuffmaneTable tabla[])
{
   for(int i=0;i<256;i++) //recorro la tabla
   {
      if(tabla[i].n!=0) //tabla[i].n es el contador de ocurrencias, si esta en 0, no aparece en el archivo
      {
         HuffmanTreeInfo huf;

         huf.c=i; //huf.c es el int correspondiente al char en ascii
         huf.n=tabla[i].n; //huf.n es el contador de ocurrencias

         //para el arbol
         huf.left=NULL;
         huf.right=NULL;

         listOrderedInsert<HuffmanTreeInfo>(lista,huf,cmpHufHuf); //agrego el huf a la lista
      }
   }
}

HuffmanTreeInfo* crearArbol(List<HuffmanTreeInfo>& lista)
{
   //creo el contador para nombrar los nodos nuevos
   //como no existe un int que sea *1, le ponemos nros que no corresponden a nada en ascii
   unsigned int nombre=256;

   //reseteo la lista para iterar
   listReset<HuffmanTreeInfo>(lista);

   //repito hasta que la lista quede con un unico elemento
   while(listSize<HuffmanTreeInfo>(lista)>1)
   {
      //parte 1: elimino los primeros dos
      HuffmanTreeInfo* h1=new HuffmanTreeInfo; //reservo el espacio para h1
      *h1=listRemoveFirst<HuffmanTreeInfo>(lista);

      HuffmanTreeInfo* h2=new HuffmanTreeInfo; //reservo el espacio para h2
      *h2=listRemoveFirst<HuffmanTreeInfo>(lista);

      //parte 2: crear un nuevo nodo que tenga como hijos a los que elimine
      HuffmanTreeInfo nuevo;
      nuevo.c=nombre; // nombre del nuevo nodo (*1)
      nuevo.left=h1;
      nuevo.right=h2;
      nuevo.n=h1->n + h2->n; //suma de los dos contadores de ocurrencias

      //parte 3: insertarlo ordenadamente en la lista
      listOrderedInsert<HuffmanTreeInfo>(lista,nuevo,cmpHufHuf);
      nombre++;

      //reseteo para volver al primer elemento
      listReset<HuffmanTreeInfo>(lista);
   }

   return listNext<HuffmanTreeInfo>(lista); //devuelvo el último elemento que me quedó (la raíz)
}

void cargarCodigosEnTabla(HuffmanTreeInfo* raiz,HuffmaneTable tabla[])
{
   HuffmanTree ht=huffmanTree(raiz); //inicializo el árbol
   string codigo="";

   // itero mientras queden hojas por visitar
   while(huffmanTreeHasNext(ht))
   {
      // obtengo un puntero a la proxima hoja y en "codigo" esta su codigo correspondiente
      HuffmanTreeInfo* x=huffmanTreeNext(ht,codigo);

      //busco en la tabla el char que corresponde a esa hoja en el arbol y cargo el codigo
      tabla[x->c].code=codigo; //x->c es el char en ascii
   }
}

void grabarArchivoComprimido(string fName,HuffmaneTable tabla[],int cantHojas)
{
   FILE* f=fopen(fName.c_str(),"r+b"); //archivo original

   string s=fName+".huf"; //le agrego el .huf al final del nombre del archivo

   FILE* fComprimido=fopen(s.c_str(),"w+b"); //archivo nuevo que voy a grabar

   // parte 1
   unsigned char cH=intToChar(cantHojas);
   write<unsigned char>(fComprimido,cH); //grabo en el primer byte del archivo la cantidad de hojas

   for(int i=0;i<256;i++)
   {
      if(tabla[i].n!=0) // nos fijamos si el caracter aparece en el file
      {
         //parte 2 (t registros)
         write<unsigned char>(fComprimido,i); //escribo el caracter en el file

         unsigned char longCode=intToChar(length(tabla[i].code));
         write<unsigned char>(fComprimido,longCode); //escribo la longitud del código en el file

         for(int j=0;j<length(tabla[i].code);j++) //recorro char a char el codigo
         {
            write<unsigned char>(fComprimido,(unsigned char)tabla[i].code[j]); //escribo el código en el file
         }
      }
   }

   //parte 3
   write<unsigned long>(fComprimido,(unsigned long)fileSize<unsigned char>(f)); // escribo la longitud del archivo original en 4 bytes

   // parte 4
   BitWriter bw=bitWriterCreate(fComprimido);

   seek<char>(f,0); //pongo el puntero de lectura/escritura en la posicion 0

   unsigned char c=read<unsigned char>(f);

   while(!feof(f))
   {
      string s=tabla[(int)c].code; //busco en la tabla el codigo del char que lei

      for(int i=0;s[i]!='\0';i++) //leo el codigo bit a bit y lo voy escribiendo
      {
         int caracter=charToInt(char(s[i]));
         bitWriterWrite(bw,caracter);
      }

      c=read<unsigned char>(f);
   }

   bitWriterFlush(bw); //relleno si quedaron bytes sin completar

   fclose(f);
   fclose(fComprimido);
}

void comprimir(string fName)
{
   // paso 1
   HuffmaneTable tabla[256]; //creo la tabla

   for(int i=0;i<256;i++)
   {
      tabla[i].n=0; //inicializo el contador en 0
      tabla[i].code=""; //inicializo el código vacío
   }

   contarOcurrencias(fName,tabla); //agrego las ocurrencias a la tabla

   // paso 2
   List<HuffmanTreeInfo> lista=list<HuffmanTreeInfo>();
   crearLista(lista,tabla); //creo la lista enlazada

   // cantidad de elementos de la lista=cantidad de hojas que quedan al final
   int cantHojas=listSize<HuffmanTreeInfo>(lista);

   // paso 3
   HuffmanTreeInfo* raiz=crearArbol(lista);

   // Recorro el arbol para obtener los códigos
   cargarCodigosEnTabla(raiz,tabla);

   // Grabo el archivo comprimido
   grabarArchivoComprimido(fName,tabla,cantHojas);
}

void reconstruirTabla(FILE* f,HuffmaneTable tabla[],unsigned int cantHojas)
{
   for(unsigned int z=0;z<cantHojas;z++)
   {
      unsigned int c=read<unsigned char>(f); // ascii del char

      unsigned int longCodigo=charToInt(read<unsigned char>(f)); // longitud del codigo de ese char

      //recupero el codigo
      string codigo="";
      for(unsigned int h=0;h<longCodigo;h++)
      {
         char m=read<char>(f);

         codigo=codigo+m; // codigo es el codigo recuperado
      }

      tabla[c].code=codigo; //agrego el código de el char "c" a la tabla
   }
}

HuffmanTreeInfo* reconstruirArbol(HuffmaneTable tabla[])
{
   //con la tabla reconstruyo el arbol sin tener en cuenta las ocurrencias

   //inicializo la raiz del arbol vacía
   HuffmanTreeInfo* raiz=new HuffmanTreeInfo; //reservo el espacio en memoria para la raiz
   raiz->left=NULL;
   raiz->right=NULL;

   for(int i=0;i<256;i++)
   {
      if(tabla[i].code!="") // solo me interesan los que aparezcan en el archivo
      {
         //reservo espacio para una variable auxiliar para poder volver a la raíz del árbol
         HuffmanTreeInfo* aux=new HuffmanTreeInfo;
         aux=raiz;

         //leo bit a bit el código del char en la tabla
         for(int j=0;j<length(tabla[i].code);j++)
         {
            if((tabla[i].code)[j]=='0') //si es 0 voy al hijo izquierdo
            {
               if(aux->left==NULL) //si no existe el hijo izquierdo, lo creo
               {
                  HuffmanTreeInfo* izq=new HuffmanTreeInfo;
                  izq->c=(char)i; //izq->c es el char en ascii
                  izq->left=NULL;
                  izq->right=NULL;

                  aux->left=izq; //lo agrego como hijo izquierdo
               }

               aux=aux->left; //me posiciono en el hijo izquierdo
            }
            else
               if(tabla[i].code[j]=='1') //si es 1 voy al hijo derecho
               {
                  if(aux->right==NULL) //si no existe el hijo izquierdo, lo creo
                  {
                     HuffmanTreeInfo* der=new HuffmanTreeInfo;
                     der->c=(char)i; //der->c es el char en ascii
                     der->left=NULL;
                     der->right=NULL;

                     aux->right=der; //lo agrego como hijo derecho
                  }

                  aux=aux->right; //me posiciono en el hijo derecho
               }
         }
      }
   }
   return raiz;
}

void descompresion(FILE* f, FILE* fDescomprimido,unsigned int longArchivoOriginal,HuffmanTreeInfo* raiz)
{
   BitReader br=bitReaderCreate(f);

   unsigned int contadorHojas=0;

   while(contadorHojas<longArchivoOriginal)
   {
      HuffmanTreeInfo* aux=raiz; //me posiciono en la raiz

      while(aux->left!=NULL and aux->right!=NULL) //recorro el arbol hasta llegar a una hoja
      {
         unsigned int bit=bitReaderRead(br);

         if(bit==1) //si es 1 voy al hijo derecho
         {
            aux=aux->right;
         }
         else //sino voy al hijo izquierdo
         {
            aux=aux->left;
         }
      }

      write<unsigned char>(fDescomprimido,aux->c); //escribo el char en el archivo descomprimido

      contadorHojas++;
   }
}

void descomprimir(string fName)
{
   FILE* f=fopen(fName.c_str(),"r+b"); //archivo original
   string s=substring(fName,0,length(fName)-4); // borro el ".huf" del archivo
   FILE* fDescomprimido=fopen(s.c_str(),"w+b"); //archivo nuevo que voy a grabar

   unsigned char cH=read<unsigned char>(f);
   unsigned int cantHojas=charToInt(cH); //me guardo la cantidad de hojas

   HuffmaneTable tabla[256]; //inicializo la tabla

   reconstruirTabla(f,tabla,cantHojas);

   HuffmanTreeInfo* raiz=reconstruirArbol(tabla);

   // 4 bytes que indican la longitud del archivo original
   unsigned int longArchivoOriginal=read<unsigned int>(f);

   descompresion(f,fDescomprimido,longArchivoOriginal,raiz);

   fclose(f);
   fclose(fDescomprimido);
}

// PROGRAMA PRINCIPAL
int main(int argc,char** argv)
{
   string fName=argv[1];
   cout<<fName<<endl;

   if(!endsWith(fName,".huf"))
   {
      cout<<"Comprimiendo..."<<endl;
      comprimir(fName);
      cout<<"Comprimido"<<endl;
   }
   else
   {
      cout<<"Descomprimiendo..."<<endl;
      descomprimir(fName);
      cout<<"Descomprimido"<<endl;
   }

   return 0;
}

#endif

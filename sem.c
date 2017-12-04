# include <stdio.h>
# include "cc.h"
# include "semutil.h"
# include "sem.h"
# include "sym.h"

extern int formalnum;
extern char formaltypes[];
extern int localnum;
extern char localtypes[];
extern int localwidths[];

int numlabels = 0;                      /* total labels in file */
int numblabels = 0;                     /* toal backpatch labels in file */

/*
 * backpatch - backpatch list of quadruples starting at p with k
 */
void backpatch(struct sem_rec *p, int k)
{
   printf( "B%d = L%d\n", p->s_place, k);
   p->s_place = k;
}

/*
 * bgnstmt - encountered the beginning of a statement
 */
void bgnstmt()
{
  extern int lineno;

  printf("bgnstmt %d\n", lineno);
  //   fprintf(stderr, "sem: bgnstmt not implemented\n");
}

/*
 * call - procedure invocation
 */
struct sem_rec *call(char *f, struct sem_rec *args)
{
  // Count number of arguments
  int numArgs = 0;
  struct sem_rec *p = args;
  while( p != NULL )
  {
    numArgs++;
    p = p->back.s_link;
  }

  // Store the number of arguments
  struct sem_rec *argArray[numArgs];
  p = args;
  int i = 0;
  while( i < numArgs )
  {
    argArray[i] = p;
    i++;
    p = p->back.s_link;
  }

  //Print the information
  for( int i = numArgs - 1; i >= 0; i-- )
  {
    if( argArray[i]->s_mode & T_INT || argArray[i]->s_mode & T_STR )
    {
      printf( "argi t%d\n", argArray[i]->s_place );
    }
    else if( argArray[i]->s_mode & T_DOUBLE )
    {
      printf( "argf t%d\n", argArray[i]->s_place );
    }
    else
    {
      fprintf(stderr, "type not supported\n");
    }
  }

  // Lookup function in the symbol table and create if not found
  struct id_entry *lLookup = lookup( f, 0 );
  if( lLookup == NULL )
  {
    lLookup = install( f,-1 );
    lLookup->i_defined = 1;
    lLookup->i_scope = GLOBAL;
    lLookup->i_type = T_INT;
  }

  // Create new sem rec for the function call and print intermediate
  int lTempFuncionCall = nexttemp();
  printf( "t%d := global %s\n", lTempFuncionCall, f );

  char lType;
  if( lLookup->i_type & T_INT )
  {
    lType = 'i';
  }
  else if( lLookup->i_type & T_DOUBLE )
  {
    lType = 'f';
  }

  struct sem_rec *newSemRec = node( nexttemp(), lLookup->i_type, NULL, NULL);

  // Print intermediate code
  printf( "t%d := f%c t%d %d\n", newSemRec->s_place, lType, lTempFuncionCall, numArgs );

  return newSemRec;
}

/*
 * ccand - logical and
 */
struct sem_rec *ccand(struct sem_rec *e1, int m, struct sem_rec *e2)
{
   fprintf(stderr, "sem: ccand not implemented\n");
   return ((struct sem_rec *) NULL);
}

/*
 * ccexpr - convert arithmetic expression to logical expression
 */
struct sem_rec *ccexpr(struct sem_rec *e)
{
   struct sem_rec *t1;

   if(e){

     t1 = gen("!=", e, cast(con("0"), e->s_mode), e->s_mode);

     printf("bt t%d B%d\n", t1->s_place, ++numblabels);
     printf("br B%d\n", ++numblabels);
     return (node(0, 0,
		  node(numblabels-1, 0, (struct sem_rec *) NULL,
		       (struct sem_rec *) NULL),
		  node(numblabels, 0, (struct sem_rec *) NULL,
		       (struct sem_rec *) NULL)));
   }
   else
     fprintf(stderr, "Argument sem_rec is NULL\n");
}

/*
 * ccnot - logical not
 */
struct sem_rec *ccnot(struct sem_rec *e)
{
   fprintf(stderr, "sem: ccnot not implemented\n");
   return ((struct sem_rec *) NULL);
}

/*
 * ccor - logical or
 */
struct sem_rec *ccor(struct sem_rec *e1, int m, struct sem_rec *e2)
{
   fprintf(stderr, "sem: ccor not implemented\n");
   return ((struct sem_rec *) NULL);
}

/*
 * con - constant reference in an expression
 */
struct sem_rec *con(char *x)
{
  struct id_entry *p;

  if((p = lookup(x, 0)) == NULL) {
    p = install(x, 0);
    p->i_type = T_INT;
    p->i_scope = GLOBAL;
    p->i_defined = 1;
  }

  /* print the quad t%d = const */
  printf("t%d := %s\n", nexttemp(), x);

  /* construct a new node corresponding to this constant generation
     into a temporary. This will allow this temporary to be referenced
     in an expression later*/
  return(node(currtemp(), p->i_type, (struct sem_rec *) NULL,
	      (struct sem_rec *) NULL));
}

/*
 * dobreak - break statement
 */
void dobreak()
{
   fprintf(stderr, "sem: dobreak not implemented\n");
}

/*
 * docontinue - continue statement
 */
void docontinue()
{
   fprintf(stderr, "sem: docontinue not implemented\n");
}

/*
 * dodo - do statement
 */
void dodo(int m1, int m2, struct sem_rec *e, int m3)
{
   fprintf(stderr, "sem: dodo not implemented\n");
}

/*
 * dofor - for statement
 */
void dofor(int m1, struct sem_rec *e2, int m2, struct sem_rec *n1,
           int m3, struct sem_rec *n2, int m4)
{
  backpatch(e2->back.s_true, m3);
  backpatch(e2->s_false, m4);
  backpatch(n1, m1);
  backpatch(n2, m2);
}

/*
 * dogoto - goto statement
 */
void dogoto(char *id)
{
   fprintf(stderr, "sem: dogoto not implemented\n");
}

/*
 * doif - one-arm if statement
 */
void doif(struct sem_rec *e, int m1, int m2)
{
  backpatch(e->back.s_true,m1);
  backpatch(e->s_false, m2);
}

/*
 * doifelse - if then else statement
 */
void doifelse(struct sem_rec *e, int m1, struct sem_rec *n,
                         int m2, int m3)
{
   fprintf(stderr, "sem: doifelse not implemented\n");
}

/*
 * doret - return statement
 */
void doret(struct sem_rec *e)
{
  char lTypeChar;
  if( e->s_mode & T_INT )
  {
    lTypeChar = 'i';
  }
  else if( e->s_mode & T_DOUBLE )
  {
    lTypeChar = 'f';
  }
  else if( e->s_mode & T_STR )
  {
    lTypeChar = 's';
  }
  else
  {
    fprintf(stderr, "Type not supported\n");
  }

  printf( "ret%c t%d\n", lTypeChar, e->s_place );
}

/*
 * dowhile - while statement
 */
void dowhile(int m1, struct sem_rec *e, int m2, struct sem_rec *n,
             int m3)
{
   fprintf(stderr, "sem: dowhile not implemented\n");
}

/*
 * endloopscope - end the scope for a loop
 */
void endloopscope(int m)
{
   fprintf(stderr, "sem: endloopscope not implemented\n");
}

/*
 * exprs - form a list of expressions
 */
struct sem_rec *exprs(struct sem_rec *l, struct sem_rec *e)
{
  e->back.s_link = l;

  return e;
}

/*
 * fhead - beginning of function body
 */
void fhead(struct id_entry *p)
{
  // Print intermediate
  for( int i = 0; i < formalnum; i++ )
  {
    if( formaltypes[i] == 'i' )
    {
      printf( "formal %lld\n", tsize(T_INT) );
    }
    else if ( formaltypes[i] == 'f' )
    {
      printf( "formal %lld\n", tsize(T_DOUBLE) );
    }
    else
    {
      fprintf(stderr, "unexpected type '%c'\n", formaltypes[i]);
    }
  }

  for( int i = 0; i < localnum; i++ )
  {
    if( localtypes[i] == 'i' )
    {
      printf( "localloc %lld\n", tsize(T_INT) );
    }
    else if ( localtypes[i] == 'f' )
    {
      printf( "localloc %lld\n", tsize(T_DOUBLE) );
    }
    else
    {
      fprintf(stderr, "unexpected type '%c'\n", localtypes[i]);
    }
  }
}

/*
 * fname - function declaration
 */
struct id_entry *fname(int t, char *id)
{
  struct id_entry *p;
  // Add to symbol table
  if ((p = lookup(id, 0)) == NULL) {
     p = install(id, -1);
     p->i_type = t;
     p->i_scope = GLOBAL;
     p->i_defined = 1;
  }
  // Print function name
  printf( "func %s\n", id );

  enterblock();

  return p;
}

/*
 * ftail - end of function body
 */
void ftail()
{
  leaveblock();
  leaveblock();
  printf( "fend\n" );
}

/*
 * id - variable reference
 */
struct sem_rec *id(char *x)
{
   struct id_entry *p;

   if ((p = lookup(x, 0)) == NULL) {
      yyerror("undeclared identifier");
      p = install(x, -1);
      p->i_type = T_INT;
      p->i_scope = LOCAL;
      p->i_defined = 1;
   }
   if (p->i_scope == GLOBAL)
      printf("t%d := global %s\n", nexttemp(), x);
   else if (p->i_scope == LOCAL)
      printf("t%d := local %d\n", nexttemp(), p->i_offset);
   else if (p->i_scope == PARAM) {
      printf("t%d := param %d\n", nexttemp(), p->i_offset);
      if (p->i_type & T_ARRAY) {
         (void) nexttemp();
         printf("t%d := @i t%d\n", currtemp(), currtemp()-1);
      }
   }

   /* add the T_ADDR to know that it is still an address */
   return (node(currtemp(), p->i_type|T_ADDR, (struct sem_rec *) NULL,
                (struct sem_rec *) NULL));
}

/*
 * index - subscript
 */
struct sem_rec *tom_index(struct sem_rec *x, struct sem_rec *i)
{
  return (gen("[]", x, cast(i, T_INT), x->s_mode&~(T_ARRAY)));
}

/*
 * labeldcl - process a label declaration
 */
void labeldcl(char *id)
{
   fprintf(stderr, "sem: labeldcl not implemented\n");
}

/*
 * m - generate label and return next temporary number
 */
int m()
{
  numlabels++;
  printf( "label L%d\n", numlabels );
  return numlabels;
}

/*
 * n - generate goto and return backpatch pointer
 */
struct sem_rec *n()
{
  numblabels++;
  printf( "br B%d\n", numblabels );
  return node( numblabels, T_LBL, NULL, NULL );
}

/*
 * op1 - unary operators
 */
struct sem_rec *op1(char *op, struct sem_rec *y)
{
  if (*op == '@' && !(y->s_mode&T_ARRAY)){
    /* get rid of T_ADDR if it is being dereferenced so can handle
       T_DOUBLE types correctly */
    y->s_mode &= ~T_ADDR;
    return (gen(op, (struct sem_rec *) NULL, y, y->s_mode));
  }
  else{
    return (gen(op, (struct sem_rec*) NULL, y, y->s_mode));
  }
}

/*
 * op2 - arithmetic operators
 */
struct sem_rec *op2(char *op, struct sem_rec *x, struct sem_rec *y)
{
  struct sem_rec *lCasty = y;
  struct sem_rec *lCastx = x;

  if( x->s_mode != y->s_mode )
  {
    if( (x->s_mode & T_DOUBLE) && !(y->s_mode & T_DOUBLE) )
    {
      lCasty = cast( y, x->s_mode );
    }
    else if( (y->s_mode & T_DOUBLE) && !(x->s_mode & T_DOUBLE) )
    {
      lCastx = cast( x, y->s_mode );
    }
    else
    {
      fprintf(stderr, "sem: rel type conversion not supported\n");
    }
  }

   return (gen(op, lCastx, lCasty, lCasty->s_mode));
}

/*
 * opb - bitwise operators
 */
struct sem_rec *opb(char *op, struct sem_rec *x, struct sem_rec *y)
{
   fprintf(stderr, "sem: opb not implemented\n");
   return ((struct sem_rec *) NULL);
}

/*
 * rel - relational operators
 */
struct sem_rec *rel(char *op, struct sem_rec *x, struct sem_rec *y)
{
  struct sem_rec *lCasty = y;
  struct sem_rec *lCastx = x;

  if( x->s_mode != y->s_mode )
  {
    if( (x->s_mode & T_DOUBLE) && !(y->s_mode & T_DOUBLE) )
    {
      lCasty = cast( y, x->s_mode );
    }
    else if( (y->s_mode & T_DOUBLE) && !(x->s_mode & T_DOUBLE) )
    {
      lCastx = cast( x, y->s_mode );
    }
    else
    {
      fprintf(stderr, "sem: rel type conversion not supported\n");
    }
  }

  struct sem_rec * lNewSemRec = gen(op, lCastx, lCasty, lCasty->s_mode);
  numblabels++;
  printf( "bt t%d B%d\n", lNewSemRec->s_place, numblabels );
  lNewSemRec->back.s_true = node( numblabels,-1, NULL, NULL );

  numblabels++;
  printf( "br B%d\n", numblabels);
  lNewSemRec->s_false = node( numblabels,-1, NULL, NULL );


  return lNewSemRec;
}

/*
 * set - assignment operators
 */
struct sem_rec *set(char *op, struct sem_rec *x, struct sem_rec *y)
{
  /* assign the value of expression y to the lval x */
  struct sem_rec *p, *cast_y;

  if(*op!='\0' || x==NULL || y==NULL){
      cast_y = op1 ("@", x);
      p = gen (op, cast_y, y, x -> s_mode);

      char lTypeChar = ( x->s_mode & T_DOUBLE ) ? 'f' : 'i';
      printf("t%d := t%d =%c t%d\n", nexttemp(), x -> s_place, lTypeChar, p -> s_place);

      return node (currtemp (), (x -> s_mode & ~(T_ARRAY)), 0, 0) ;
  }

  /* if for type consistency of x and y */
  cast_y = y;
  if((x->s_mode & T_DOUBLE) && !(y->s_mode & T_DOUBLE)){

    /*cast y to a double*/
    printf("t%d := cvf t%d\n", nexttemp(), y->s_place);
    cast_y = node(currtemp(), T_DOUBLE, (struct sem_rec *) NULL,
		  (struct sem_rec *) NULL);
  }
  else if((x->s_mode & T_INT) && !(y->s_mode & T_INT)){

    /*convert y to integer*/
    printf("t%d := cvi t%d\n", nexttemp(), y->s_place);
    cast_y = node(currtemp(), T_INT, (struct sem_rec *) NULL,
		  (struct sem_rec *) NULL);
  }

  /*output quad for assignment*/
  if(x->s_mode & T_DOUBLE)
    printf("t%d := t%d =f t%d\n", nexttemp(),
	   x->s_place, cast_y->s_place);
  else
    printf("t%d := t%d =i t%d\n", nexttemp(),
	   x->s_place, cast_y->s_place);

  /*create a new node to allow just created temporary to be referenced later */
  return(node(currtemp(), (x->s_mode&~(T_ARRAY)),
	      (struct sem_rec *)NULL, (struct sem_rec *)NULL));
}

/*
 * startloopscope - start the scope for a loop
 */
void startloopscope()
{
   enterblock();
}

/*
 * string - generate code for a string
 */
struct sem_rec *string(char *s)
{
  printf( "t%d := %s\n", nexttemp(), s );

  // Create heap space for a new semantic record
  struct sem_rec *p = (struct sem_rec *)malloc(sizeof(struct sem_rec));
  p->s_place = currtemp();
  p->s_mode = T_STR;

  return p;
}



/************* Helper Functions **************/

/*
 * cast - force conversion of datum y to type t
 */
struct sem_rec *cast(struct sem_rec *y, int t)
{
   if (t == T_DOUBLE && y->s_mode != T_DOUBLE)
      return (gen("cv", (struct sem_rec *) NULL, y, t));
   else if (t != T_DOUBLE && y->s_mode == T_DOUBLE)
      return (gen("cv", (struct sem_rec *) NULL, y, t));
   else
      return (y);
}

/*
 * gen - generate and return quadruple "z := x op y"
 */
struct sem_rec *gen(char *op, struct sem_rec *x, struct sem_rec *y, int t)
{
   if (strncmp(op, "arg", 3) != 0 && strncmp(op, "ret", 3) != 0)
      printf("t%d := ", nexttemp());
   if (x != NULL && *op != 'f')
      printf("t%d ", x->s_place);
   printf("%s", op);
   if (t & T_DOUBLE && (!(t & T_ADDR) || (*op == '[' && *(op+1) == ']'))) {
      printf("f");
      if (*op == '%')
         yyerror("cannot %% floating-point values");
   }
   else
      printf("i");
   if (x != NULL && *op == 'f')
      printf(" t%d %d", x->s_place, y->s_place);
   else if (y != NULL)
      printf(" t%d", y->s_place);
   printf("\n");
   return (node(currtemp(), t, (struct sem_rec *) NULL,
           (struct sem_rec *) NULL));
}

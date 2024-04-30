

#include<stdio.h>
#include<stdlib.h>



typedef struct {
                int id;  //azonosito
                long* ProcT; //muveleti idok listajara mutato pointer
                long* StartT; //inditasi idopont --||--
                long* EndT;   //befejezesi idopont --||--
               }StroctOfJobs;


typedef struct {
                long ST;
                long ET;
               }TimeWindows;

typedef struct {
                int id;  //azonosito
                long* TransT;  //anyagmozgatasi ido
                long** SetT;   //atallasi idok
                int NCal;      //intervallumok sama
                TimeWindows* Cal;  //rendelkezesre allasi idointervallumok
               }StructOfMachines;


long Max_Lateness(long a, long b) { return a>=b ? a : b; }
long Min_Lateness(long a, long b) { return a<=b ? a : b; }

void FlowShop_Simulator( StroctOfJobs* job, int NumberOfJobs, StructOfMachines* machine, int NumberOfMachines, int* s, long t0);
void prinStructOfMachines_Gantt( StroctOfJobs* job, int NumberOfJobs, int NumberOfMachines, int* s);
void prinStroctOfJobs_Gantt( StroctOfJobs* job, int NumberOfJobs, int NumberOfMachines, int* s);
void Johnsons_algorithm(StroctOfJobs* job, int NumberOfJobs, int r, int* s);
long Evaluate(StroctOfJobs* job, int NumberOfJobs, int NumberOfMachines, int* s);

void Dannenbring_algorithm(StroctOfJobs* job, int NumberOfJobs, int NumberOfMachines, int* s);
void CopyScheduling( int* s1, int* s2, int NumberOfJobs );
void CDS_algorithm(StroctOfJobs* job, int NumberOfJobs, StructOfMachines* machine, int NumberOfMachines, int* s);

void PrinStructOfMachines_Cal(StructOfMachines* machine, int NumberOfMachines);
int Load_STET_to_Cal(long* st, long* et, StructOfMachines* machine, int r);











int main(int argc, char* argv[])
{
  int NumberOfJobs; //munkak szama
  int i, j;  //munka indexe
  int NumberOfMachines; //munkahelyek (eroforrasok) szama
  int r, k;  //munkahely indexe
  StroctOfJobs* job;
  StructOfMachines* machine;
  int* s; //kozos utemterv minden gepre, mert a munkak nem elozheti meg egymast
  int r_value;
  int c;  //idointervallum index

  printf(" \n Flow shop modell");
  printf("\n Munkak szama=");
  scanf("%d", &NumberOfJobs);

  printf("\n Munkahelyek szama=");
  scanf("%d", &NumberOfMachines);

  machine = (StructOfMachines*) calloc(NumberOfMachines, sizeof(StructOfMachines) ); //eroforrasok
  for( r=0; r<NumberOfMachines; r++)
    {
      machine[r].id = r;
      machine[r].TransT = (long*) calloc(NumberOfMachines, sizeof(long) );  //vektor
      for( k=0; k<NumberOfMachines; k++)
         machine[r].TransT[k] = 10 + rand()%20;

      machine[r].SetT = (long**) calloc(NumberOfJobs, sizeof(long*) );
      for ( i=0; i<NumberOfJobs; i++ )
       {
        machine[r].SetT[i] = (long*) calloc(NumberOfJobs, sizeof(long));
        for (j=0; j<NumberOfJobs; j++)
           if (i == j)
               machine[r].SetT[i][j] = 0;
           else
               machine[r].SetT[i][j] = 10 + rand()%100; //sorrendfuggo atallasi idok
       }

     //idointervallumok
     machine[r].NCal = 2 + rand()%10;
     machine[r].Cal = (TimeWindows*) calloc(machine[r].NCal, sizeof(TimeWindows) ); //mem. fogl.
     for ( c=0; c<machine[r].NCal; c++)
       {  //nem lapolodhatnak at
          if ( c== 0 )
              machine[r].Cal[c].ST = 10 + rand()%30;
          else
              machine[r].Cal[c].ST = machine[r].Cal[c-1].ET + rand()%30;

          machine[r].Cal[c].ET = machine[r].Cal[c].ST + rand()%100;

       }

    }

  s = (int*) calloc(NumberOfJobs, sizeof(int) ); //utemterv
  job = (StroctOfJobs*) calloc(NumberOfJobs, sizeof(StroctOfJobs) );
  for( i=0; i<NumberOfJobs; i++)
   {
     job[i].id = i;
     job[i].ProcT = (long*) calloc(NumberOfMachines, sizeof(long));  //muveleti idok gepenkent
     for (r=0; r<NumberOfMachines; r++)
        job[i].ProcT[r] = 1 + rand()%100;

     job[i].StartT = (long*) calloc(NumberOfMachines, sizeof(long));  //inditasi idopontok gepenkent
     job[i].EndT = (long*) calloc(NumberOfMachines, sizeof(long));  //befejezesi idopontok gepenkent

    //i] = i; //ad-hoc sorrend
    s[i] = NumberOfJobs-i-1; //forditott ad-hoc sorrend
   }

   {   //teszt blokk
   int m;
   long s, e;
   int x;

   PrinStructOfMachines_Cal(machine, NumberOfMachines);
   printf("\n m = ");
   scanf("%d", &m);
   printf("\n s = ");
   scanf("%ld", &s);
   printf("\n e = ");
   scanf("%ld", &e);

   x = Load_STET_to_Cal(&s, &e, machine, m);

   printf("\n s = %ld", s);
   printf("\n e = %ld", e);
   printf("\n x = %d", x);




   }

   //ad-hoc sorrend szimulacioja
   printf("\n Ad-hoc sorrend");
   FlowShop_Simulator( job, NumberOfJobs, machine, NumberOfMachines, s, 0);
   //prinStructOfMachines_Gantt( job, NumberOfJobs, NumberOfMachines, s);
   //prinStroctOfJobs_Gantt( job, NumberOfJobs, NumberOfMachines, s);
   printf("\n Cmax = %ld", Evaluate(job, NumberOfJobs, NumberOfMachines, s) );

  
   //Fm|perm|Cmax
   printf("\n\n Dannenbring sorrend");
   Dannenbring_algorithm(job, NumberOfJobs, NumberOfMachines, s);
   FlowShop_Simulator( job, NumberOfJobs, machine, NumberOfMachines, s, 0);
   //prinStructOfMachines_Gantt( job, NumberOfJobs, NumberOfMachines, s);
   //prinStroctOfJobs_Gantt( job, NumberOfJobs, NumberOfMachines, s);
   printf("\n Cmax = %ld", Evaluate(job, NumberOfJobs, NumberOfMachines, s) );

   //CDS algoritmus
   printf("\n\n CDS sorrend");
   CDS_algorithm(job, NumberOfJobs, machine, NumberOfMachines, s);
   FlowShop_Simulator( job, NumberOfJobs, machine, NumberOfMachines, s, 0);
   //prinStructOfMachines_Gantt( job, NumberOfJobs, NumberOfMachines, s);
   //prinStroctOfJobs_Gantt( job, NumberOfJobs, NumberOfMachines, s);
   printf("\n Cmax = %ld", Evaluate(job, NumberOfJobs, NumberOfMachines, s) );

   

  //mem. felszabaditas
  for( i=0; i<NumberOfJobs; i++)
   {
     free( job[i].ProcT );
     free( job[i].StartT );
     free( job[i].EndT );
   }
  free( job );

  for( r=0; r<NumberOfMachines; r++)
   {
      free(machine[r].TransT);
      for( i=0; i<NumberOfJobs; i++)
        free( machine[r].SetT[i]);

      free( machine[r].Cal );  
   }
   free( machine );

    return 0;
}

void FlowShop_Simulator( StroctOfJobs* job, int NumberOfJobs, StructOfMachines* machine, int NumberOfMachines, int* s, long t0)
{
  int i; //az inditas sorrendjenek indexe
  int r; //munkahely indexe, technologiai sorrendet koveti a gepek azonositoja

  for( i=0; i<NumberOfJobs; i++ )
    for( r=0; r<NumberOfMachines; r++ )
      {
         if ( i==0 )
            { //kezdo munka
              if ( r==0 )
                { //kezdo gepen
                  job[s[i]].StartT[r] = t0;
                }
              else
                { //nem kezdo gepen
                  job[s[i]].StartT[r] = job[s[i]].EndT[r-1]  //befejezes
                                        + machine[r].TransT[r-1]; //mozgatas
                }

                       //bef = indul + atall + dolgozik;
                       //alapertelmezett beallitas kezdetben a 0 job
                job[s[i]].EndT[r] = job[s[i]].StartT[r]   //kezdes
                             + machine[r].SetT[ 0 ][ s[i] ] //atallas
                             + job[s[i]].ProcT[r];  //muvelet

            }
         else
            { //nem kezdo munka
              if ( r==0 )
                { //kezdo gepen
                  job[s[i]].StartT[r] = job[s[i-1]].EndT[r];
                }
              else
                { //nem kezdo gepen                       erkezes = befezes + mozgatas
                  job[s[i]].StartT[r] = Max_Lateness( job[s[i]].EndT[r-1] + machine[r].TransT[r-1],  //elozo geprol mikor erkezik
                                               job[s[i-1]].EndT[r] ); //elozo munka mikor fejezodik be
                }
                     //bef = indul + atall + dolgozik;
                 job[s[i]].EndT[r] = job[s[i]].StartT[r]   //kezdes
                             + machine[r].SetT[ s[i-1] ][ s[i] ] //atallas
                             + job[s[i]].ProcT[r];  //muvelet

            }


      }
}

void prinStructOfMachines_Gantt( StroctOfJobs* job, int NumberOfJobs, int NumberOfMachines, int* s)
{
  int i;
  int r;

  printf("\n Eroforras-orientalt Gantt adatok:");

  for ( r=0; r<NumberOfMachines; r++ )
  {
    printf("\n %d. munkahely:", r);
    printf("\n # \t munka \t indul \t muv.\t bef.");
    for ( i=0; i<NumberOfJobs; i++ )
      printf("\n %d\t%d\t%ld\t%ld\t%ld",
                 i,
                 s[i],
                 job[s[i]].StartT[r],
                 job[s[i]].ProcT[r],
                 job[s[i]].EndT[r] );

  }
}

void prinStroctOfJobs_Gantt( StroctOfJobs* job, int NumberOfJobs, int NumberOfMachines, int* s)
{
  int i;
  int r;

  printf("\n\n Munka-orientalt Gantt adatok:");

  for ( i=0; i<NumberOfJobs; i++ )
  {
    printf("\n %d. munka vegrehajtasa:", s[i] );
    printf("\n # \t gep \t indul \t muv.\t bef.");
    for ( r=0; r<NumberOfMachines; r++ )
      printf("\n %d\t%d\t%ld\t%ld\t%ld",
                 r,
                 r,
                 job[s[i]].StartT[r],
                 job[s[i]].ProcT[r],
                 job[s[i]].EndT[r] );

  }
}

void Johnsons_algorithm(StroctOfJobs* job, int NumberOfJobs, int r, int* s)
{
  int i, j;
  int index, temp;
  long value, val_of_j;
  int* v;      //elorendezes
  int first, last;  //szabad helyek indexe

  v = (int*) calloc(NumberOfJobs, sizeof(int));

  for ( i=0; i<NumberOfJobs; i++)
     v[i]=i;

  //elorendezes
  for(i=0; i<NumberOfJobs-1; i++)
  {
    index = i;
    value = Min_Lateness( job[ v[i] ].ProcT[r], job[ v[i] ].ProcT[r+1]);
    for(j=i+1; j<NumberOfJobs; j++)
     {
       val_of_j = Min_Lateness( job[ v[j] ].ProcT[r], job[ v[j] ].ProcT[r+1]);

      if ( val_of_j < value )
         {
           index = j;
           value = val_of_j;
         }
     }

    if ( index != i ) //van jobb jelolt
      {  //csere
         temp = v[ index ];
         v[ index ] = v[ i ];
         v[ i ] = temp;
      }
  }

  //utemezes
  first = 0;    //eleje
  last = NumberOfJobs-1;  //vege

  for( i=0; i<NumberOfJobs; i++)
  {
     if ( job[ v[i] ].ProcT[r] <= job[ v[i] ].ProcT[r+1] )
       { //elorol haladva a mar beutemezettek moge
         s[ first ] = v[i];
         first++;
       }
     else
     {  //hatulrol haladva a beutemezettek ele
        s[ last ] = v[i];
        last--;

     }

  }

  free( v );
}

long Evaluate(StroctOfJobs* job, int NumberOfJobs, int NumberOfMachines, int* s)
{
   //kihasznalva az F|perm|Cmax modell jellemzoit
   //Cmax = utolsokent inditott munka bef. ideje az utolso gepen
   return job[ s[NumberOfJobs-1]].EndT[NumberOfMachines-1];
}



void Dannenbring_algorithm(StroctOfJobs* job, int NumberOfJobs, int NumberOfMachines, int* s)
{

  int i;  //job index
  int j;  //gep es operacio index
  StroctOfJobs* virStroctOfJobs;
  //virtualis F2 rendszer
  virStroctOfJobs = (StroctOfJobs*) calloc(NumberOfJobs, sizeof(StroctOfJobs));
  for ( i=0; i<NumberOfJobs; i++ )
   {
      virStroctOfJobs[i].ProcT = (long*) calloc(2, sizeof(long));

      //idoadatok transzformacioja
      virStroctOfJobs[i].ProcT[0] = 0;
      virStroctOfJobs[i].ProcT[1] = 0;

      for( j=0; j<NumberOfMachines; j++)
       {
         virStroctOfJobs[i].ProcT[0] += job[i].ProcT[j] * (NumberOfMachines-(j+1) + 1);
         virStroctOfJobs[i].ProcT[1] += job[i].ProcT[j] * (j+1);
       }
   }

   //Johnson algoritmus alkalmazása a virtuális környezetben
   Johnsons_algorithm(virStroctOfJobs, NumberOfJobs, 0, s);

   //mem. felszabaditas
   for( i=0; i<NumberOfJobs; i++ )
      free( virStroctOfJobs[i].ProcT );

   free( virStroctOfJobs );
}

void CDS_algorithm(StroctOfJobs* job, int NumberOfJobs, StructOfMachines* machine, int NumberOfMachines, int* s)
{
  int i; //munka
  int r; //gep

  int* s_act;  //utemterv
  int* s_best;  //legjobb vizsgalt utemterv

  long C_act, C_best;

  s_act = (int*) calloc( NumberOfJobs, sizeof(int) );
  s_best = (int*) calloc( NumberOfJobs, sizeof(int) );

  for ( r=0; r<NumberOfMachines; r++)
    {
      //r es r+1 virtualis ketgepes feladat
      //Johnson
      Johnsons_algorithm(job, NumberOfJobs, r, s_act);
      //szimulacio
      FlowShop_Simulator( job, NumberOfJobs, machine, NumberOfMachines, s_act, 0);
      //kiertekeles
      C_act = Evaluate(job, NumberOfJobs, NumberOfMachines, s_act);

      if ( r == 0 )
         {
           C_best = C_act;
           CopyScheduling( s_best, s_act, NumberOfJobs);
         }
      else
         {
           if ( C_best > C_act )
             {
               C_best = C_act;
               CopyScheduling( s_best, s_act, NumberOfJobs);
             }
         }
    }

   CopyScheduling( s, s_best, NumberOfJobs);


  free( s_act );
  free( s_best );

}

void CopyScheduling( int* s1, int* s2, int NumberOfJobs )
{
  int i;

  for ( i=0; i<NumberOfJobs; i++ )
    s1[i] = s2[i];
}




void PrinStructOfMachines_Cal(StructOfMachines* machine, int NumberOfMachines)
{
   int r;
    int c;

   printf("\n\n Eroforrasok rendelkezesre allasi idointervallumai");
   for (r=0; r<NumberOfMachines; r++)
     {
       printf("\n %d. eroforras [%d]", r, machine[r].NCal );
       printf("\n # \t Kezdet\tVege");
       for (c=0; c<machine[r].NCal; c++)
         printf("\n %d \t %ld \t %ld", c, machine[r].Cal[c].ST, machine[r].Cal[c].ET);
    }
}

int Load_STET_to_Cal(long* st, long* et, StructOfMachines* machine, int r)
{
   //megszakitas nelkuli elhelyezes

  int c;  //intervallum index
  long new_st;  //modositott kezdet
  long new_et;  //modositott befejezes
  long size;    //muvelet vegrehajtasi ideje
  int found;    //megtalalt alkalmas intervallum indexe

  new_st = *st;
  new_et = *et;
  size = *et - *st;

  found = -1; //nincs talalat
  c = 0;

  while( c < machine[r].NCal )
    {
      if ( new_st < machine[r].Cal[c].ET ) //vizsgalhato
        {
           //hatarra illesztes ha szukseges
           new_st = Max_Lateness( new_st, machine[r].Cal[c].ST);
           new_et = new_st + size;   //vegenek az illesztese

           if ( new_et <= machine[r].Cal[c].ET )
             {  //belefer, kesz
                found = c;
                break;
             }
           else
             { //kilog
               c++;
               if ( c >= machine[r].NCal )
                 {
                    new_st = machine[r].Cal[c-1].ET;
                    new_et = new_st + size;
                    break;
                 }


               continue;
             }

        }
      c++;

    }

    *st = new_st;
    *et = new_et;

    return found;
}


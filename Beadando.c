

#include<stdio.h>
#include<stdlib.h>



typedef struct {
                int id;  //azonosito
                long* ProcT; //muveleti idok listajara mutato pointer
                long* StartT; //inditasi idopont --||--
                long* EndT;   //befejezesi idopont --||--
               }T_JOB;


typedef struct {
                long ST;
                long ET;
               }T_TIMEWINDOW;

typedef struct {
                int id;  //azonosito
                long* TransT;  //anyagmozgatasi ido
                long** SetT;   //atallasi idok
                int NCal;      //intervallumok sama
                T_TIMEWINDOW* Cal;  //rendelkezesre allasi idointervallumok
               }T_RES;


long max_l(long a, long b) { return a>=b ? a : b; }
long min_l(long a, long b) { return a<=b ? a : b; }

void Simulation_FS( T_JOB* job, int NJ, T_RES* res, int NR, int* s, long t0);
void print_Res_Gantt( T_JOB* job, int NJ, int NR, int* s);
void print_Job_Gantt( T_JOB* job, int NJ, int NR, int* s);
void Johnson_alg(T_JOB* job, int NJ, int r, int* s);
long Evaluate(T_JOB* job, int NJ, int NR, int* s);
int F3_Johnson_alg_ext(T_JOB* job, int NJ, int r, int* s);
int Fm_Palmer_alg(T_JOB* job, int NJ, int NR, int* s);
void Fm_Dannenbring_alg(T_JOB* job, int NJ, int NR, int* s);
void copy_sch( int* s1, int* s2, int NJ );
void Fm_CDS_alg(T_JOB* job, int NJ, T_RES* res, int NR, int* s);
void Neighbour(int* s_0, int* s_act, int NJ);
void Fm_Search_alg(T_JOB* job, int NJ, T_RES* res, int NR, int* s, int STEP, int LOOP);
void Print_Res_Cal(T_RES* res, int NR);
int Load_STET_to_Cal(long* st, long* et, T_RES* res, int r);











int main(int argc, char* argv[])
{
  int NJ; //munkak szama
  int i, j;  //munka indexe
  int NR; //munkahelyek (eroforrasok) szama
  int r, k;  //munkahely indexe
  T_JOB* job;
  T_RES* res;
  int* s; //kozos utemterv minden gepre, mert a munkak nem elozheti meg egymast
  int r_value;
  int c;  //idointervallum index

  printf(" \n Flow shop modell");
  printf("\n Munkak szama=");
  scanf("%d", &NJ);

  printf("\n Munkahelyek szama=");
  scanf("%d", &NR);

  res = (T_RES*) calloc(NR, sizeof(T_RES) ); //eroforrasok
  for( r=0; r<NR; r++)
    {
      res[r].id = r;
      res[r].TransT = (long*) calloc(NR, sizeof(long) );  //vektor
      for( k=0; k<NR; k++)
         res[r].TransT[k] = 10 + rand()%20;

      res[r].SetT = (long**) calloc(NJ, sizeof(long*) );
      for ( i=0; i<NJ; i++ )
       {
        res[r].SetT[i] = (long*) calloc(NJ, sizeof(long));
        for (j=0; j<NJ; j++)
           if (i == j)
               res[r].SetT[i][j] = 0;
           else
               res[r].SetT[i][j] = 10 + rand()%100; //sorrendfuggo atallasi idok
       }

     //idointervallumok
     res[r].NCal = 2 + rand()%10;
     res[r].Cal = (T_TIMEWINDOW*) calloc(res[r].NCal, sizeof(T_TIMEWINDOW) ); //mem. fogl.
     for ( c=0; c<res[r].NCal; c++)
       {  //nem lapolodhatnak at
          if ( c== 0 )
              res[r].Cal[c].ST = 10 + rand()%30;
          else
              res[r].Cal[c].ST = res[r].Cal[c-1].ET + rand()%30;

          res[r].Cal[c].ET = res[r].Cal[c].ST + rand()%100;

       }

    }

  s = (int*) calloc(NJ, sizeof(int) ); //utemterv
  job = (T_JOB*) calloc(NJ, sizeof(T_JOB) );
  for( i=0; i<NJ; i++)
   {
     job[i].id = i;
     job[i].ProcT = (long*) calloc(NR, sizeof(long));  //muveleti idok gepenkent
     for (r=0; r<NR; r++)
        job[i].ProcT[r] = 1 + rand()%100;

     job[i].StartT = (long*) calloc(NR, sizeof(long));  //inditasi idopontok gepenkent
     job[i].EndT = (long*) calloc(NR, sizeof(long));  //befejezesi idopontok gepenkent

    //i] = i; //ad-hoc sorrend
    s[i] = NJ-i-1; //forditott ad-hoc sorrend
   }

   {   //teszt blokk
   int m;
   long s, e;
   int x;

   Print_Res_Cal(res, NR);
   printf("\n m = ");
   scanf("%d", &m);
   printf("\n s = ");
   scanf("%ld", &s);
   printf("\n e = ");
   scanf("%ld", &e);

   x = Load_STET_to_Cal(&s, &e, res, m);

   printf("\n s = %ld", s);
   printf("\n e = %ld", e);
   printf("\n x = %d", x);




   }
  

   //ad-hoc sorrend szimulacioja
   printf("\n Ad-hoc sorrend");
   Simulation_FS( job, NJ, res, NR, s, 0);
   //print_Res_Gantt( job, NJ, NR, s);
   //print_Job_Gantt( job, NJ, NR, s);
   printf("\n Cmax = %ld", Evaluate(job, NJ, NR, s) );

   if ( NR == 2 )
    { //F2|perm|Cmax
      printf("\n Jonhson sorrend");
      Johnson_alg(job, NJ, 0, s);
      Simulation_FS( job, NJ, res, NR, s, 0);
      //print_Res_Gantt( job, NJ, NR, s);
      //print_Job_Gantt( job, NJ, NR, s);
      printf("\n Cmax = %ld", Evaluate(job, NJ, NR, s) );

    }

   if ( NR == 3 )
   {  //F3|perm|Cmax
      printf("\n Kiterjesztett Jonhson sorrend");
      r_value = F3_Johnson_alg_ext(job, NJ, 0, s);
      Simulation_FS( job, NJ, res, NR, s, 0);
      print_Res_Gantt( job, NJ, NR, s);
      //print_Job_Gantt( job, NJ, NR, s);
      printf("\n Cmax = %ld", Evaluate(job, NJ, NR, s) );
      if ( r_value == 1 ) printf("\n A megoldás optimális!");
      else printf("\n Nem garantalhato az optimum elerese.");
   }

   //Fm|perm|Cmax
   printf("\n\n Palmer sorrend");
   Fm_Palmer_alg(job, NJ, NR, s);
   Simulation_FS( job, NJ, res, NR, s, 0);
   //print_Res_Gantt( job, NJ, NR, s);
   //print_Job_Gantt( job, NJ, NR, s);
   printf("\n Cmax = %ld", Evaluate(job, NJ, NR, s) );

   //Fm|perm|Cmax
   printf("\n\n Dannenbring sorrend");
   Fm_Dannenbring_alg(job, NJ, NR, s);
   Simulation_FS( job, NJ, res, NR, s, 0);
   //print_Res_Gantt( job, NJ, NR, s);
   //print_Job_Gantt( job, NJ, NR, s);
   printf("\n Cmax = %ld", Evaluate(job, NJ, NR, s) );

   //CDS algoritmus
   printf("\n\n CDS sorrend");
   Fm_CDS_alg(job, NJ, res, NR, s);
   Simulation_FS( job, NJ, res, NR, s, 0);
   //print_Res_Gantt( job, NJ, NR, s);
   //print_Job_Gantt( job, NJ, NR, s);
   printf("\n Cmax = %ld", Evaluate(job, NJ, NR, s) );

   //kereso pelda
   {
    int STEP, LOOP;
    printf("\n\n Kereso algoritmus");

    printf("\n Kereso lepesek szama STEP = ");
    scanf("%d", &STEP);
    printf("\n Szomszedok szama loop = ");
    scanf("%d", &LOOP);


    Fm_Search_alg(job, NJ, res, NR, s, STEP, LOOP);
    Simulation_FS( job, NJ, res, NR, s, 0);
   //print_Res_Gantt( job, NJ, NR, s);
   print_Job_Gantt( job, NJ, NR, s);
   printf("\n Cmax = %ld", Evaluate(job, NJ, NR, s) );
  }

  //mem. felszabaditas
  for( i=0; i<NJ; i++)
   {
     free( job[i].ProcT );
     free( job[i].StartT );
     free( job[i].EndT );
   }
  free( job );

  for( r=0; r<NR; r++)
   {
      free(res[r].TransT);
      for( i=0; i<NJ; i++)
        free( res[r].SetT[i]);

      free( res[r].Cal );  
   }
   free( res );


  return 0;
}

void Simulation_FS( T_JOB* job, int NJ, T_RES* res, int NR, int* s, long t0)
{
  int i; //az inditas sorrendjenek indexe
  int r; //munkahely indexe, technologiai sorrendet koveti a gepek azonositoja

  for( i=0; i<NJ; i++ )
    for( r=0; r<NR; r++ )
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
                                        + res[r].TransT[r-1]; //mozgatas
                }

                       //bef = indul + atall + dolgozik;
                       //alapertelmezett beallitas kezdetben a 0 job
                job[s[i]].EndT[r] = job[s[i]].StartT[r]   //kezdes
                             + res[r].SetT[ 0 ][ s[i] ] //atallas
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
                  job[s[i]].StartT[r] = max_l( job[s[i]].EndT[r-1] + res[r].TransT[r-1],  //elozo geprol mikor erkezik
                                               job[s[i-1]].EndT[r] ); //elozo munka mikor fejezodik be
                }
                     //bef = indul + atall + dolgozik;
                 job[s[i]].EndT[r] = job[s[i]].StartT[r]   //kezdes
                             + res[r].SetT[ s[i-1] ][ s[i] ] //atallas
                             + job[s[i]].ProcT[r];  //muvelet

            }


      }
}

void print_Res_Gantt( T_JOB* job, int NJ, int NR, int* s)
{
  int i;
  int r;

  printf("\n Eroforras-orientalt Gantt adatok:");

  for ( r=0; r<NR; r++ )
  {
    printf("\n %d. munkahely:", r);
    printf("\n # \t munka \t indul \t muv.\t bef.");
    for ( i=0; i<NJ; i++ )
      printf("\n %d\t%d\t%ld\t%ld\t%ld",
                 i,
                 s[i],
                 job[s[i]].StartT[r],
                 job[s[i]].ProcT[r],
                 job[s[i]].EndT[r] );

  }
}

void print_Job_Gantt( T_JOB* job, int NJ, int NR, int* s)
{
  int i;
  int r;

  printf("\n\n Munka-orientalt Gantt adatok:");

  for ( i=0; i<NJ; i++ )
  {
    printf("\n %d. munka vegrehajtasa:", s[i] );
    printf("\n # \t gep \t indul \t muv.\t bef.");
    for ( r=0; r<NR; r++ )
      printf("\n %d\t%d\t%ld\t%ld\t%ld",
                 r,
                 r,
                 job[s[i]].StartT[r],
                 job[s[i]].ProcT[r],
                 job[s[i]].EndT[r] );

  }
}

void Johnson_alg(T_JOB* job, int NJ, int r, int* s)
{
  int i, j;
  int index, temp;
  long value, val_of_j;
  int* v;      //elorendezes
  int first, last;  //szabad helyek indexe

  v = (int*) calloc(NJ, sizeof(int));

  for ( i=0; i<NJ; i++)
     v[i]=i;

  //elorendezes
  for(i=0; i<NJ-1; i++)
  {
    index = i;
    value = min_l( job[ v[i] ].ProcT[r], job[ v[i] ].ProcT[r+1]);
    for(j=i+1; j<NJ; j++)
     {
       val_of_j = min_l( job[ v[j] ].ProcT[r], job[ v[j] ].ProcT[r+1]);

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
  last = NJ-1;  //vege

  for( i=0; i<NJ; i++)
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

long Evaluate(T_JOB* job, int NJ, int NR, int* s)
{
   //kihasznalva az F|perm|Cmax modell jellemzoit
   //Cmax = utolsokent inditott munka bef. ideje az utolso gepen
   return job[ s[NJ-1]].EndT[NR-1];
}

int F3_Johnson_alg_ext(T_JOB* job, int NJ, int r, int* s)
{

  int i;  //job index
  T_JOB* virt_job;
  long min_pi0, max_pi1, min_pi2;
  //virtualis F2 rendszer
  virt_job = (T_JOB*) calloc(NJ, sizeof(T_JOB));
  for ( i=0; i<NJ; i++ )
   {
      virt_job[i].ProcT = (long*) calloc(2, sizeof(long));

      //idoadatok transzformacioja
      virt_job[i].ProcT[0] = job[i].ProcT[r] +
                             job[i].ProcT[r+1];

      virt_job[i].ProcT[1] = job[i].ProcT[r+1] +
                             job[i].ProcT[r+2];

   }

   //Johnson algoritmus alkalmazása a virtuális környezetben
   Johnson_alg(virt_job, NJ, 0, s);

   //optimalitás feltétel vizsgálata
   min_pi0 = job[0].ProcT[0];
   max_pi1 = job[0].ProcT[1];
   min_pi2 = job[0].ProcT[2];

   for( i=1; i<NJ; i++ )
    {
       if ( min_pi0 > job[i].ProcT[0] )
            min_pi0 = job[i].ProcT[0];

       if ( max_pi1 < job[i].ProcT[1] )
            max_pi1 = job[i].ProcT[1];

       if ( min_pi2 > job[i].ProcT[2] )
            min_pi2 = job[i].ProcT[2];
    }

   //mem. felszabaditas
   for( i=0; i<NJ; i++ )
      free( virt_job[i].ProcT );

   free( virt_job );

   if ( min_pi0 <= max_pi1 ||
        min_pi2 <= max_pi1
      )
        return 1; //optimalis a megoldas
   else
        return 0; //nem garantalhato az optimalis megoldas
}

int Fm_Palmer_alg(T_JOB* job, int NJ, int NR, int* s)
{
  long* I; //Palmer-index
  int i; //munka index
  int j; //operacio es gep index
  int k, temp, index;


  I = (long*) calloc(NJ, sizeof(long)); //tomb
  //Palmer-index meghatarozasa
  for ( i=0; i<NJ; i++)
    {
      I[i] = 0;
      for ( j=0; j<NR; j++)
       I[i] += -1 * job[i].ProcT[j] * (NR - (2*(j+1) - 1) ) / 2;
    }

   for( i=0; i<NJ; i++)
     s[i] = i;

   //Utemezes a Palmer-indexek alapjan
   for ( i=0; i<NJ-1; i++)
    {
       index = i;
       for ( k=i+1; k<NJ; k++)
          if ( I[ s[k] ] > I[ s[index] ] )
             index = k;

      if ( index != i )
        {  //csere
           temp = s[index];
           s[index] = s[i];
           s[i] = temp;
        }
    }

  free( I );
}

void Fm_Dannenbring_alg(T_JOB* job, int NJ, int NR, int* s)
{

  int i;  //job index
  int j;  //gep es operacio index
  T_JOB* virt_job;
  //virtualis F2 rendszer
  virt_job = (T_JOB*) calloc(NJ, sizeof(T_JOB));
  for ( i=0; i<NJ; i++ )
   {
      virt_job[i].ProcT = (long*) calloc(2, sizeof(long));

      //idoadatok transzformacioja
      virt_job[i].ProcT[0] = 0;
      virt_job[i].ProcT[1] = 0;

      for( j=0; j<NR; j++)
       {
         virt_job[i].ProcT[0] += job[i].ProcT[j] * (NR-(j+1) + 1);
         virt_job[i].ProcT[1] += job[i].ProcT[j] * (j+1);
       }
   }

   //Johnson algoritmus alkalmazása a virtuális környezetben
   Johnson_alg(virt_job, NJ, 0, s);

   //mem. felszabaditas
   for( i=0; i<NJ; i++ )
      free( virt_job[i].ProcT );

   free( virt_job );
}

void Fm_CDS_alg(T_JOB* job, int NJ, T_RES* res, int NR, int* s)
{
  int i; //munka
  int r; //gep

  int* s_act;  //utemterv
  int* s_best;  //legjobb vizsgalt utemterv

  long C_act, C_best;

  s_act = (int*) calloc( NJ, sizeof(int) );
  s_best = (int*) calloc( NJ, sizeof(int) );

  for ( r=0; r<NR; r++)
    {
      //r es r+1 virtualis ketgepes feladat
      //Johnson
      Johnson_alg(job, NJ, r, s_act);
      //szimulacio
      Simulation_FS( job, NJ, res, NR, s_act, 0);
      //kiertekeles
      C_act = Evaluate(job, NJ, NR, s_act);

      if ( r == 0 )
         {
           C_best = C_act;
           copy_sch( s_best, s_act, NJ);
         }
      else
         {
           if ( C_best > C_act )
             {
               C_best = C_act;
               copy_sch( s_best, s_act, NJ);
             }
         }
    }

   copy_sch( s, s_best, NJ);


  free( s_act );
  free( s_best );

}

void copy_sch( int* s1, int* s2, int NJ )
{
  int i;

  for ( i=0; i<NJ; i++ )
    s1[i] = s2[i];
}

void Fm_Search_alg(T_JOB* job, int NJ, T_RES* res, int NR, int* s, int STEP, int LOOP)
{
  int step;
  int loop;

  int* s_act;  //utemterv
  int* s_best;  //legjobb vizsgalt utemterv
  int* s_0;  //bazis utemterv
  int* s_ext;  //legjobb szomszed vizsgalt utemterv

  long C_act, C_best, C_ext;

  s_act = (int*) calloc( NJ, sizeof(int) );
  s_best = (int*) calloc( NJ, sizeof(int) );
  s_0 = (int*) calloc( NJ, sizeof(int) );
  s_ext = (int*) calloc( NJ, sizeof(int) );

  copy_sch(s_0, s, NJ); //kezdeti bazis
  copy_sch(s_best, s, NJ); //legjobb ismert
  Simulation_FS( job, NJ, res, NR, s_best, 0);
  C_best = Evaluate(job, NJ, NR, s_best);



  for ( step=1; step<=STEP; step++)
  {
   for ( loop = 1; loop<=LOOP; loop++ )
    {
      //szomszed generalasa
      Neighbour(s_0, s_act, NJ);
      //szimulacio
      Simulation_FS( job, NJ, res, NR, s_act, 0);
      //kiertekeles
      C_act = Evaluate(job, NJ, NR, s_act);

      if ( loop == 1 )
         { //legjobb szomszed
           C_ext = C_act;
           copy_sch( s_ext, s_act, NJ);
         }
      else
         {
           if ( C_ext > C_act )
             {
               C_ext = C_act;
               copy_sch( s_ext, s_act, NJ);
             }
         }
    }//loop

   //uj bazist ad a legjobb szomszed
   copy_sch( s_0, s_ext, NJ);

   if ( C_best > C_ext )
     {
        C_best = C_ext;
        copy_sch( s_best, s_ext, NJ);
     }

  }//step

   copy_sch( s, s_best, NJ);


  free( s_act );
  free( s_best );
  free( s_0 );
  free( s_ext );
}

void Neighbour(int* s_0, int* s_act, int NJ)
{ //pelda szomszedsagi operatorra
  int x;

  //teljes masolat
  copy_sch(s_act, s_0, NJ);
  //modositas
  x = rand()%NJ; //veletlenszeruen valasztott pozicio
  if ( x == 0 )
    {
      s_act[x] = s_0[NJ-1];
      s_act[NJ-1] = s_0[x];
    }
  else
    {
      s_act[x] = s_0[x-1];
      s_act[x-1] = s_0[x];

    }


}

void Print_Res_Cal(T_RES* res, int NR)
{
   int r;
    int c;

   printf("\n\n Eroforrasok rendelkezesre allasi idointervallumai");
   for (r=0; r<NR; r++)
     {
       printf("\n %d. eroforras [%d]", r, res[r].NCal );
       printf("\n # \t Kezdet\tVege");
       for (c=0; c<res[r].NCal; c++)
         printf("\n %d \t %ld \t %ld", c, res[r].Cal[c].ST, res[r].Cal[c].ET);
    }
}

int Load_STET_to_Cal(long* st, long* et, T_RES* res, int r)
{
   //az algoritmus megkeresi azt a legkorabbi rendelkezesre allasi
   //idointervallumot, amelyre megszakitas nelkul el tudja helyezni
   //a muveletet

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

  while( c < res[r].NCal )
    {
      if ( new_st < res[r].Cal[c].ET ) //vizsgalhato
        {
           //hatarra illesztes ha szukseges
           new_st = max_l( new_st, res[r].Cal[c].ST);
           new_et = new_st + size;   //vegenek az illesztese

           if ( new_et <= res[r].Cal[c].ET )
             {  //belefer, kesz
                found = c;
                break;
             }
           else
             { //kilog
               c++;
               if ( c >= res[r].NCal )
                 {
                    new_st = res[r].Cal[c-1].ET;
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


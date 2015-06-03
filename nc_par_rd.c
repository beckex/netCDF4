#include <stdlib.h>
#include <stdio.h>
#include <netcdf.h>

#include "mpi.h"
/* This is the name of the data file we will read. */
#define FILE_NAME "nc4.nc"
/* We are reading 2D data, a 6 x 12 grid. */
#define NT 1
#define NX 73
#define NY 144
/* Handle errors by printing an error message and exiting with a
 * non-zero status. */
#define ERRCODE 2
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); exit(ERRCODE);}

#define  MASTER   0

int
main(int argc, char** argv)
{
    int   numtasks, taskid, len;
    char hostname[MPI_MAX_PROCESSOR_NAME];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
    MPI_Get_processor_name(hostname, &len);
    printf ("Hello from task %d on %s!\n", taskid, hostname);
    if (taskid == MASTER)
       printf("MASTER: Number of MPI tasks is: %d\n",numtasks);
   /* There will be netCDF IDs for the file, each group, and each
    * variable. */
   int ncid, varid1, varid2;
   short tcw_in[NT][NX][NY];
   /* Loop indexes, and error handling. */
   int x, y, retval;
   /* The following struct is written as a compound type. */
   short tcwv_in[NT][NX][NY];

   size_t startp[]={taskid, 0, 0};
   size_t countp[]={NT, NX, NY};
   size_t stridep[]={1,1,1};
   /* Open the file. NC_NOWRITE tells netCDF we want read-only access
    * to the file.*/
   if ((retval = nc_open_par(FILE_NAME, NC_MPIIO|NC_NETCDF4, MPI_COMM_WORLD, MPI_INFO_NULL, &ncid)))
      ERR(retval);
   /* Get the varid of the uint64 data variable, based on its name, in
    * grp1. */
   if ((retval = nc_inq_varid(ncid, "tcw", &varid1))) 
      ERR(retval);
   /* Read the data. */
   if ((retval = nc_get_vars_short(ncid, varid1, startp, countp, stridep, &tcw_in[0][0][0])))
      ERR(retval);
   /* Get the varid of the compound data variable, based on its name,
    * in grp2. */
   if ((retval = nc_inq_varid(ncid, "tcwv", &varid2))) 
      ERR(retval);
   /* Read the data. */
   if ((retval = nc_get_vars_short(ncid, varid2, startp, countp, stridep, &tcwv_in[0][0][0])))
      ERR(retval);
   /* Check the data. */
   int t=0;
   for(t=0;t<NT;t++){
    printf("%d ", tcw_in[t][0][0]);
   }
   printf("\n");
   for(t=0;t<NT;t++){
    printf("%d ", tcwv_in[t][0][0]);
   }
   printf("\n");
   /* Close the file, freeing all resources. */
   if ((retval = nc_close(ncid)))
      ERR(retval);
   printf("*** SUCCESS reading example file %s from task %d!\n", FILE_NAME, taskid);
   
   MPI_Finalize();
   return 0;
}

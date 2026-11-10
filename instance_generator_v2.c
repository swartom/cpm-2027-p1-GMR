#include <stdio.h>
#include <stdlib.h>
#include <time.h>
int main(int argc, char *argv[]) {
    
    int m = atoi(argv[1]);
    char * file_name = argv[2];
    if(m < 1)
        exit(1);    
    char c[2] ; int** h = malloc(sizeof(int*) *m);
    for(int i  =0; i < m;i ++)
        h[i] = malloc(sizeof(int)*m);
    
    FILE* f = fopen(file_name,"r");
    int i = 0;
    int j = 0;
    for(int i =0;i < m ;i ++)
        for(int j =0;j < m ;j ++)
            h[i][j] = 0;
    while(fgets(c, 2, f)) {
        
        if(c[0] == '\n'){            
            i+=1, j=-1;
        }
        else if (c[0] == ' ')
            h[i][j] = 1;
        
        j+=1;
    }

    fclose(f);
    
    remove("data.txt");
    f = fopen("data.txt","w");
    int count = 0;
    for (int i =0; i < m; i ++) {
        for (int j = 0; j < m ;j++ ) {
            if (h[i][j]==1)
                fprintf(f,"%d\n%d\n",i,j), count +=1;
                /* fprintf(f,"%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n",i,j,i+1,j,i,j+1,i+1,j+1); */
        }
    }
    fclose(f);
    remove("info.txt");
    f = fopen("info.txt","w");
    fprintf(f,"%d",count);
    fclose(f);
    remove("lkh_solver.txt");
    f = fopen("lkh_solver.txt","w");
    
    fprintf(f,"NAME : example\nTYPE : TSP\nDIMENSION : %d\nEDGE_WEIGHT_TYPE : EUC_2D\nNODE_COORD_SECTION\n", count);
    count = 0;
    for (int i =0; i < m; i ++) {
        for (int j = 0; j < m ;j++ ) {
            if (h[i][j]==1)
                fprintf(f,"%d %d %d\n",count+1,i*20,j*20), count +=1;
                /* fprintf(f,"%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n",i,j,i+1,j,i,j+1,i+1,j+1); */
        }
    }
    fprintf(f,"EOF");
    fclose(f);
    free(h);
    return 0;
}

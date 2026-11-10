#include "main.h"
#include <stdlib.h>
#include <time.h>
#define RECORD_EXECUTION_TIME 
/* #include <stdint.h> */
/* #define PRINT_COLOUR_COUNT */
/* #define DEBUG */
#ifdef DEBUG
/* #define DEBUG_2 */
#endif
/* #define GCODE // Output G-Code */

#define UNION_FIND // Increases memory, reduces compute time
#define DFS_STRATEGY 
/* #define SINGLE_STEP_SWEEPLINE // Step the sweepline only one at each pass */
/* #define ABLATION_STUDY */
/* #define OUTPUT_K */

/* #define REORIENTATOR */
#ifdef REORIENTATOR
#define CHECK_WIDTH 50
#endif
#define CASE_THREE // Enable / disable the edge-case generator
#define DROOPING_PAIRS // Drooping pairs
#define POST_EXPAND // Expand after all work is done.

int main(int argc, char *argv[]) {
    int m = atoi(argv[1]);
    int z = atoi(argv[2]);
    int rotation = atoi(argv[3]);    
    
    char * file_name   = argv[3];
    if(m < 1) 
        exit(0);
    FILE* f = fopen(file_name,"r");    
    char c[2]; int v[2]; bool b = 0; v[0] = 0;v[1] = 0;    
    bool** h = malloc(sizeof(bool*) *(m+1));
    int square_counter = 0;
    for(int i  =0; i < m+1;i ++)
        h[i] = malloc(sizeof(bool)*(m+1));
    
    for (int i =0; i < m+1; i ++)
        for (int j = 0; j < m+1 ;j++ )
            if (z == 0)
                h[i][j] = 0;
            else
                h[j][i] = 0;
    while(fgets(c, 2, f)) {
        if (c[0] == '\n') {
            b= !b;
            if (b == 0) h[v[0]][v[1]] = 1;
            v[b]=0;
        }
        else  v[b] *= 10, v[b] += c[0]-48;
    }    
    fclose(f);    
    
    bool** r = malloc(sizeof(bool*) *(m+1));
    for(int i  =0; i < m+1;i ++)
        r[i] = malloc(sizeof(bool)*(m+1));
    
    for (int i =0; i < m; i ++)
        for (int j = 0; j < m ;j++ )
            r[i][j] = 0;

    clock_t begin = clock();    
    for (int i =0; i < m; i +=
             #ifndef SINGLE_STEP_SWEEPLINE 
             2
             #endif
             #ifdef  SINGLE_STEP_SWEEPLINE
             1
             #endif
    ){
        bool need_connection = 1; 
        for (int j = 0; j < m;j++ ){
            #define has_square(a,b)                                     \
            (r[a][b]) | (h[a][b] & h[a+1][b] & h[a][b+1] & h[a+1][b+1])
            r[i][j] =  has_square(i,j);
            // Eliminate marker if present
            if (r[i][j] == 1 ) h[i][j] = 0, h[i+1][j]=0;
            /* if (need_connection == 1 && i > 2 && r[i][j] == 1 && r[i-2][j] == 1 ) { */
            /*     r[i-1][j]=1,need_connection = 0;                 */
            /* } */
            if (r[i][j] == 1 && j < m-1 && i < m-1 && (!h[i][j+2] || !h[i+1][j+2]) ){
                h[i][j+1] = 0, h[i+1][j+1]=0;
                #ifdef REORIENTATOR                 
                int boundary = j > CHECK_WIDTH ? CHECK_WIDTH : j;                
                bool mark = 0;
                int back_step_point = 0;
                for (int k = j-1; k > j-CHECK_WIDTH;k--)
                    if (r[i][k] == 1)                            
                        back_step_point +=1;                            
                    else{
                        mark = 1;
                        break;                            
                    }
                if ((j-back_step_point)%2 == 1) back_step_point +=1;                    
                if(mark ) for (int k = j-back_step_point;k < j ;k+=2 )
                              if(r[i][k]) {r[i][k] = 0; if(k==j-back_step_point) h[i][k] = 1, h[i+1][k]=1;}                
                #endif                
            }
            // If its larger, then check the sub-graph matches to K_2 in C_3 in the square..
            #ifdef CASE_THREE
            /* // Return to a graph square argument .. */
            if( i < m-2 && j > 0)                
                if(!r[i][j] && !(has_square(i+2,j)) &&
                   r[i][j-1] &&  h[i+2][j] & h[i+1][j+1] & h[i+2][j+1] && h[i+2][j-1])
                    h[i+2][j] = 0, h[i+1][j+1] = 0, h[i+2][j+1] = 0, h[i+2][j-1] = 0,
                    r[i+1][j-1] = 1, r[i+1][j] = 1;             
            #endif
        }        
    }
        
    #ifdef DROOPING_PAIRS
    for (int i = 2; i < m; i ++){
        for (int j = 0; j < m;j++ ){
            if (r[i-2][j])
                r[i-1][j] =  r[i-1][j] | (h[i][j] & h[i][j+1]);
            else if (r[i+1][j])
                r[i][j] =  r[i][j] | (h[i][j] & h[i][j+1]);
            if (r[i-1][j] == 1 || r[i][j] == 1) h[i][j] = 0,h[i][j+1] = 0; 
            
        }
    }
    #endif        
    int colour = 1;
    int colour_minimum_mapping = 0;
    for (int i =0; i < m-1; i ++) {
        bool marker  = 0;
        for (int j = 0; j < m-1;j++ ) 
            marker = marker | r[i][j];
        if (marker) colour_minimum_mapping += 1;
    }
    
    int last_colour_mapper = 0;
    int* colour_sizes = malloc(sizeof(int*) *(m+1));
    int** k = malloc(sizeof(int*) *(m+1));
    
    for(int i  =0; i < m+1;i ++) k[i] = malloc(sizeof(int)*(m+1));
    
    for (int i =0; i < m; i ++) for (int j = 0; j < m;j++ ) k[i][j] = 0;

    for (int i =0; i < m; i ++) {
            
        for (int j = 0; j < m;j++ ) 
            if (r[i][j] == 1) {                
                k[i][j] = colour;
                if (j > 0 && k[i][j-1] > 0) k[i][j]=k[i][j-1]; // Note 1.
                else if (i > 0 && k[i-1][j] > 0) k[i][j]=k[i-1][j];
                else k[i][j]= colour, colour = colour + 1;                
            }
        
    }
    int* parent = malloc(sizeof(int)*colour);
    int* rank = malloc(sizeof(int)*colour);
    int* ij_marks = malloc(2*sizeof(int)*colour); 
    for (int i  =0; i < colour; i++) parent[i] = i, rank[i] = 0;    
    inline int find_set(int v) {
        if (v == parent[v])
            return v;        
        parent[v] = find_set(parent[v]);
        return parent[v];
    }
    inline void update_ij_of_set(int v, int i,int j) {
        v = find_set(v),
        ij_marks[2*v] = i, ij_marks[2*v +1] = j;
    }
    inline void union_set(int v,int u) {
        int a = find_set(v), b = find_set(u);        
        if (a != b) {
            if (rank[a] < rank[b])
                a ^= b, b^= a, a ^=b;             
            parent[b]=a;
            if (rank[a] == rank[b]) rank[a]+=1;
        } 
    }    

    for (int i =0; i < m; i ++)
        for (int j = 0; j < m;j++ ) {
            int v = find_set(k[i][j]);
            if (k[i][j] >0 && k[i+1][j] > 0 && find_set(k[i+1][j]) != find_set(k[i][j]))
                union_set(k[i+1][j],k[i][j]), update_ij_of_set(k[i][j],i,j);
            if(ij_marks[2*v] == 0 && ij_marks[2*v +1] == 0)
                update_ij_of_set(k[i][j],i,j);
        }
    
    for (int i =0; i < m-2; i ++) for (int j = 0; j < m;j++ )
        if(k[i][j] > 0 &&  k[i+2][j] > 0 && find_set(k[i][j]) != find_set(k[i+2][j]))
            k[i+1][j] = k[i][j], union_set(k[i][j],k[i+2][j]), update_ij_of_set(k[i][j],i,j);

    for (int i =0; i < m; i ++) for (int j = 0; j < m-2;j++ )
        if(k[i][j] > 0 &&  k[i][j+2] > 0 && find_set(k[i][j]) != find_set(k[i][j+2]))
            k[i][j+1] = k[i][j], union_set(k[i][j],k[i][j+2]), update_ij_of_set(k[i][j],i,j);
    
    for (int i =0; i < m-2; i ++) for (int j = 0; j < m;j++ )
        if(find_set(k[i][j]) != find_set(k[i+2][j]) && k[i][j] > 0 &&  k[i+2][j] > 0)
            k[i+1][j] = k[i][j], union_set(k[i][j],k[i+2][j]), update_ij_of_set(k[i][j],i,j);

    inline int degree(int a,int b) {
// This is probably only artefact behaviour
// which can be removed later on..
        printf("Degree_called, %d,%d\n",a,b);
        int x1 = a+1 < m ? k[a+1][b] != 0: 0,
        x2 = a-1 > -1 ? k[a-1][b] != 0: 0,
        x3 = b+1 < m ? k[a][b+1] != 0: 0,
        x4 = b-1 > -1 ? k[a][b-1] != 0: 0;
        return x1 +x2 +x3+x4;
    }
    
    for (int i =0; i < m-1; i ++){
        for (int j = 0; j < m-1;j++ ){
            // Using non logical means we can short circuit..
            if(k[i][j] && k[i+1][j] && k[i][j+1] && k[i+1][j+1]){
                /* #ifdef DEBUG */
                printf("t:%d,%d,[%d]\n",i,j,degree(i,j));
                /* #endif */
                int a = 0,b = 0;
                // Due to construction, only one square can be of
                // degree two, so find it via listing...
                if(degree(i,j) == 2) a = i, b = j;
                else if(degree(i+1,j) == 2)  a = i+1, b = j;
                else if(degree(i,j+1) == 2)  a = i, b = j+1;
                else if(degree(i+1,j+1) == 2) a = i+1, b = j+1;
                k[a][b] = 0;
            }
        }
    }
    /* #endif  */
    #ifdef POST_EXPAND
    for (int i = 0; i < m; i ++){
        for (int j = 1; j < m-1;j++ ){
            if(h[i][j] && h[i+1][j]){                
                if (k[i][j-2] > 0)
                    k[i][j-1] = k[i][j-2];
                else if (k[i][j+1] > 0)
                    k[i][j] = k[i][j+1];
                h[i][j] =0 , h[i+1][j] =0;
            }
        }
    }
    #endif

    // Enumerate all unique subgraphs, in the structure of the UF data-structure,

    // Logical error is here checking number of union finds
    // rather than number of elements in the structure, does not always co-encide...
    int* uniqueness_check  = malloc(sizeof(int) * colour);
    int* size  = malloc(sizeof(int) * colour);
    // Noting that 0 is no element at all
    for (int i =0; i < colour;i ++) size[i] = 0;     
    for (int i =0; i < m-1; i ++) for (int j = 0; j < m-1;j++ ) size[k[i][j]] +=1;
    for (int i =0; i < colour;i ++) uniqueness_check[i] = 0;
    for (int i =1; i < colour;i ++) {        
        uniqueness_check[find_set(i)] += size[i];
    }
    int index_of_max = 0;
    int max = 0;
    /* printf("colours:%d\n",colour ); */
    for (int i =0; i < colour;i ++){
        /* printf("ID:%d,%d\n", i,uniqueness_check[i]); */
        if (uniqueness_check[i] > 0){            
            /* printf("ID:%d,%d\n", i,uniqueness_check[i]); */
            if (uniqueness_check[i] > max){ 
                max = uniqueness_check[i],
                    index_of_max = i;                            
            }
        }
    }
    int x = ij_marks[2*index_of_max],y = ij_marks[2*index_of_max +1];
    
    /* printf("MAX:%d,%d, Color: %d\n",index_of_max,max, k[x][y]); */
    free(parent),free(rank),free(uniqueness_check),free(ij_marks),free(size),free(colour_sizes);
    #ifdef DEBUG
    printf("%d,%d\n\n", x,y);    
    for (int i =0; i < m; i ++){ 
        for (int j = 0; j < m;j++ ){
            if (k[i][j] >= 1) printf(" ");
            else printf("x");
                   }
        printf("(%d) \n",i);
    }
    #endif    
    
    enum {BOTTOM = 0, RIGHT = 1, TOP = 2, LEFT = 3};
    struct list {
        unsigned int id : 2;
        int a, b;
        int x,y;
        struct list* next;
        struct list* last;
        bool k[2];
        bool terminated;
        bool is_start_of_memoryallocation_block[2];
    }__attribute__((packed));
    int length = 5;
    struct list* root = malloc(sizeof(struct list)*4);        
    for(int i =0; i < 4;i ++){        
        root[i].a = x, root[i].b = y, root[i].id = i;
        (&root[i])->next = &root[(i+1)%4];
        (&root[i])->last = &root[(i+4)%4];
    }    
    root[0].x = x, root[1].x = x+1, root[3].x = x,root[2].x= x+1;
    root[0].y = y, root[1].y = y, root[3].y = y+1,root[2].y= y+1;            
    struct list* start = &root[0];
    
    #ifdef DEBUG
    void print_orientation(struct list* r) {
        switch (r->id) {
            case BOTTOM: printf("bottom\n"); break;
            case TOP: printf("top\n"); break;
            case LEFT: printf("left\n"); break;
            case RIGHT: printf("right\n"); break;
        }
    }
#endif
    
    // This is the main dummy-frame..
    inline bool extend(struct list* r){        
        int direction = r->id;
        int a = 0, b = 0;
        #ifdef ABLATION_STUDY
        int x1 = 0, x2 = 0, y1 = 0, y2 = 0;
        #endif
        switch (direction) {
            case LEFT:   a = 0, b = -1; break;
            case RIGHT:  a = 0, b = 1; break;
            case TOP:    a = 1, b = 0; break;
            case BOTTOM: a = -1, b = 0; break;
            default: exit(0); // Error
        }        
        if(
           r->a + a >= 0 && r->a + a < m
           && r->b + b >= 0 && r->b + b < m
           && k[r->a + a][r->b + b] 
        ) {
            k[r->a + a][r->b + b] = 0;
            struct list* prio = (struct list* ) malloc(sizeof(struct list)*2);            
            prio->k[0]= true,prio->k[1]=true;          
            for(int i =0; i < 2;i ++)
                prio[i].a = r->a + a, prio[i].b = r->b+b,
                    (&prio[i])->next = &prio[(i+1)%2], (&prio[i])->last = &prio[(i+2)%2];
            prio[0].last = r, prio[1].next = r->last->next;
            // Update the positional data of the pushing square
            r->a = prio[0].a, r->b = prio[0].b;
            r->next->last = &prio[1];
            prio[1].next = r->next;
            r->next = &prio[0];
            prio[0].last = r;
            prio[0].y = prio[0].last->y+a, prio[0].x = prio[0].last->x+b;
            prio[1].x = prio[1].next->x+b, prio[1].y = prio[1].next->y+a;
            prio[0].id = r->id, r->id = (r->id + 3)%4,prio[1].id = (prio[0].id +1)%4;
            length +=2;
            return 1;                
        }        
        return 0;
    }
    
    
    
    #ifdef DFS_STRATEGY
    inline void dfs(struct list * n) {
        struct list * child_pointer_1 = n->next;
        struct list * child_pointer_2 = n->next->next;
        if(extend(n))
            dfs(n);        
        if(extend(child_pointer_1))
            dfs(child_pointer_1);
        if(extend(child_pointer_2))
            dfs(child_pointer_2);
    }
    for (int i = 0; i < 4; i++)  dfs(&root[i]);          
    #endif    
    #ifndef DFS_STRATEGY
    struct list* current = start;
    bool changed =0;
    do {
        changed = 0;
        do {
            if(extend(current)){
                /* struct list* x1 = */
                changed = 1;
            }
            current = current->next;
        } while(current != start);
    }while(changed);
#endif
    
    clock_t end = clock();
    int clock_cycles = end-begin;
    /* remove("clock_cycle.txt"); */
    /* f = fopen("clock_cycle.txt","w"), fprintf(f,"%d\n%.8f\n",clock_cycles,((double)clock_cycles)/CLOCKS_PER_SEC),fclose(f); */
    remove("points_parsed.txt");
    
    struct list* printer = start;    
    remove("info2.txt");
    remove("info.txt");
    f = fopen("info.txt","w"), fprintf(f,"%d\n%d\n%d\n%d",printer->x,printer->y,printer->a,printer->b),fclose(f);
    f = fopen("info2.txt","w"), fprintf(f,"%d\n%d\n",colour,colour_minimum_mapping),fclose(f);
    f = fopen("points_parsed.txt","w");
    struct list** pointer_free_list_controller = malloc(sizeof(struct list*)*length);
    int point_stack_list_j = 0;
    
    int maximumx = 0, maximumy = 0;
    for(int i = 0; i < length;i ++)
    {
        if(printer->k[0] & printer->k[1])            
            pointer_free_list_controller[point_stack_list_j] = printer,
            point_stack_list_j +=1;        
        fprintf(f,"%d,%d\n",printer->x,printer->y), printer = printer->next, maximumx = printer->x > maximumx ? printer->x : maximumx,maximumy = printer->y > maximumy ? printer->y : maximumy;        
    }
    /* printf("%d\n",point_stack_list_j ); */
    for (int i = 0; i < point_stack_list_j; i++) {
        /* printf("%d,%d,%b,%b\n",pointer_free_list_controller[i]->x,pointer_free_list_controller[i]->y,pointer_free_list_controller[i]->k[0],pointer_free_list_controller[i]->k[1]); */
        free(pointer_free_list_controller[i]);
    }
    free(root);
    free(pointer_free_list_controller);
    fclose(f);
    
    #ifdef GCODE
    remove("gcode_out.gcode");
    f = fopen("gcode_out.gcode","w");
    int precision = 50;
    fprintf(f,"// Input: %dx%d\n// Points: %d\n// Precision : %.5fmm\n// On Lattice: %.5fmm^2 \n",m,m,length,precision,((double)1)/precision,((double)1)/precision * m);
    fprintf(f,"O0001\n");
    int last_x = 0;
    int last_y = 0;
    for(int i = 0; i < length;i ++) {
        if(rotation)
            fprintf(f,"G1 X%.5f Y%.5f Z%.5f\n",(double)(abs(printer->x))/precision,(double)(abs(printer->y))/precision,((double)z)/precision);
        else
            fprintf(f,"G1 X%.5f Y%.5f Z%.5f\n",(double)(abs(printer->y))/precision,(double)(abs(printer->x))/precision,((double)z)/precision);

        printer = printer->next;
    }
    fclose(f);
    #endif    
    for(int i =0; i < m+1; i ++)
        free(h[i]), free(k[i]),free(r[i]);
    free(h),free(k),free(r);
    return 0;
}

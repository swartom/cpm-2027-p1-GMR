#include <stdlib.h>
#include <time.h>
#include <stdio.h>

// This is a implementation of an stc map constructed via rastering 2x2 squares and then dfsing on that grid graph.
int main(int argc, char *argv[]) {
    int m = atoi(argv[1]);
    int z = atoi(argv[2]);
    int rotation = atoi(argv[3]);    
    
    char * file_name   = argv[3];
    if(m < 1)
        exit(0);
    FILE* f = fopen(file_name,"r");    
    char c[2]; int v[2]; bool b = 0; v[0] = 0;v[1] = 0;
    /* int h[m+1][m+1]; */
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


    bool** r = malloc(sizeof(bool*) *(m/2));
    for(int i  =0; i < m/2;i ++)
        r[i] = malloc(sizeof(bool)*(m/2));
    clock_t begin = clock();
    for(int i =0; i < m; i +=2)
        for(int j =0; j < m; j +=2) {
            r[i/2][j/2] = 0;
            #define has_square(a,b)                                     \
            (r[a/2][b/2]) | (h[a][b] & h[a+1][b] & h[a][b+1] & h[a+1][b+1])
            r[i/2][j/2] = has_square(i,j);
            if (r[i/2][j/2])
                h[i][j] = 0, h[i+1][j] = 0, h[i][j+1] = 0, h[i+1][j+1] = 0;            
        }
    

    struct node_tree {
        int id;
        int x,y;
        bool visited;
        struct node_tree* parent;
        struct node_tree* child[4];        
    };
    
    struct node_tree* nil = malloc(sizeof(struct node_tree));
    int id_marker = 0;
    inline void dfs(int i, int j,struct node_tree* parent,int index) {
        
        if(r[i][j] == 1){
            id_marker +=1;            
            r[i][j] = 0;
            struct node_tree* me = malloc(sizeof(struct node_tree));
            me->id = id_marker;
        me->x  = i;
        me->y = j;
        me->visited = false;
        me->parent = parent;
        me->parent->child[index] = me;
        for(int i =0;i < 4; i ++) me->child[i] = nil;                                                
        if (j - 1 >= 0 ) dfs(i,j-1,me,3);    
        if (i - 1 >= 0 ) dfs(i-1,j,me,2);
        if (j + 1 < m )  dfs(i,j+1,me,1);
        if (i + 1 < m )  dfs(i+1,j,me,0);                           
        }
    }
    int e = 0, d =0;
    for (int i  =5; i < m/2;i ++ ) {
        for (int j  =0; j < m/2;j ++ ) {
            if (r[i][j] == 1){
                e = i, d = j;
            }
        }
        if (e > 1 && e != 0 && d != 0)
            break;
    }

    struct node_tree* root = malloc(sizeof(struct node_tree));
    int index = 3;
    dfs(e,d,root,index);
    struct node_tree* pos = root->child[index];
    
    remove("stc_output.txt");
    f = fopen("stc_output.txt","w");
    int count = 0;
    inline void dfstree(struct node_tree* current) {
        count +=1;
        current->visited = true;
        int back_to_parent = 0; 
        for(int i =0; i < 4; i ++) {
            if(current->parent->child[i] == current){
                back_to_parent = (i +2) % 4;
                break;
            }
        }        
        int x = 2*current->x, y = 2*current->y;
        for(int i =1; i < 4; i ++){
            int j = (back_to_parent + i) % 4;            
            switch(j) {
                case 0: fprintf(f,"%d,%d\n",x+1,y);break;
                case 1: fprintf(f,"%d,%d\n",x+1,y+1);break;
                case 2: fprintf(f,"%d,%d\n",x,y+1); break;
                case 3: fprintf(f,"%d,%d\n",x,y);break;                                                           
            }
            if (current->child[j] != nil && !current->child[j]->visited)        
                dfstree(current->child[j]);                
        }
        return; 
    }
    dfstree(pos);
    clock_t end = clock();    
    fclose(f);
}

#include "gmr.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct line_segment {float a,b,c,d;};

struct slab {
  int y;
  struct line_segment** sorted_list;
};

struct polygon {
  struct line_segment* segments;
  int length;
  // Sorted Lists of pointers
  struct line_segment** x_sorted;
  struct line_segment** y_sorted;   
  // Slab decomposition by the
  // vertices of the polygon.
  int slab_count;
  struct slab* slabs;
  struct polygon* tool1_contour;
  struct polygon* tool2_contour;
};

// Spin this out into build polygon and extend polygon

struct polygon* build_polygon(FILE* f,int length) {
  struct line_segment* segments = malloc(sizeof(struct line_segment)*length);
  int comparison_linesegments_x(const void *a, const void* b) {
    struct line_segment* x = *(struct line_segment**)a;                  
    struct line_segment* y = *(struct line_segment**)b;    
    if ((x->a < y->a && x->a < y->c) || (x->c < y->a && x->c < y->c))
      return -1;
    else
      return 1;
    return 0;
  }
  int comparison_linesegments_y(const void *a, const void* b) {
    struct line_segment* x = *(struct line_segment**)a;    
              
    struct line_segment* y = *(struct line_segment**)b;    
    if ((x->b < y->b && x->b < y->d) || (x->d < y->a && x->d < y->d))
      return -1;
    else
      return 1;
    return 0;
  }
  
  char c[2]; int index = 0; bool b = false; int v[2];
  v[0] = 0; v[1] = 0;
  int counter = 0;
  while(fgets(c, 2, f)) {    
    if (c[0] == ',' || c[0]=='\n') {      
            b= !b;
            if (b == 0) {
              if (index % 2 == 0)
                segments[index/2].a = (float) v[0], segments[index/2].b = (float) v[1];
              else
                segments[index/2].c = (float) v[0], segments[index/2].d = (float) v[1],
                segments[index/2 + 1].a = (float) v[0], segments[index/2+ 1].b = (float) v[1],
                index += 1;
              index += 1;
              counter +=1;
            }
            v[b]=0;
        } else  v[b] *= 10, v[b] += c[0]-48;
    if (c[0]=='\n') // End of polygon chain
      break;
  }  
  segments[index/2].c = segments[0].a,segments[index/2].d = segments[0].b;
  
  struct polygon* wrapper = malloc(sizeof(struct polygon));
  wrapper->segments = segments;
  wrapper->length = counter;
  // given at this point we can extract the unique
  // count of vertices at this stage prior to sorting
  int* y_slabs = malloc(sizeof(int)*wrapper->length);
  for (int i = 0; i < wrapper->length; i++) {
    y_slabs[i]=wrapper->segments[i].b;
  }
  int comp(const void * x, const void * y) {
    int a = *(int*)x, b = *(int*)y;    
    return a -b;
  }
  qsort(y_slabs,wrapper->length, sizeof(int),comp);    
  struct line_segment** x_sort = malloc(sizeof(struct line_segment)*wrapper->length);
  struct line_segment** y_sort = malloc(sizeof(struct line_segment)*wrapper->length);
  for (int i =0; i < wrapper->length; i++)
x_sort[i] = &segments[i], y_sort[i] = &segments[i];
  qsort(x_sort,wrapper->length,sizeof(x_sort[0]),comparison_linesegments_x);
  qsort(y_sort,wrapper->length,sizeof(y_sort[0]),comparison_linesegments_y);
  wrapper->x_sorted = x_sort, wrapper->y_sorted = y_sort;

  // We allocate a slab for each vertices and draw a
  // horizontal line for each
  struct slab* slabs = malloc(sizeof(struct slab)*wrapper->length);
  wrapper->slabs = slabs;
  int l = 0;
  for (int i  =0; i < wrapper->length; i++,l++) {
    wrapper->slabs[l].y = y_slabs[i];
    if (wrapper->slabs[l-1].y == wrapper->slabs[l].y) l-=1;
  }
  wrapper->slab_count = l;
  int sum_total = 0;
  for (int i =0; i < wrapper->slab_count-1;i++ ) {
    wrapper->slabs[i].sorted_list = malloc(sizeof(struct line_segment*)*wrapper->length);
    int start_pos = wrapper->slabs[i].y;
    int end_pos = wrapper->slabs[i+1].y;
    
    /* printf("%d,%d\n", start_pos,end_pos);     */
    int k = 0;    
    for(int j =0; j < wrapper->length;j++) {
      struct line_segment* segment = &wrapper->segments[j];      
      // is segment in this slab?
      if (
        // Sweepline hangs on the start position
          (segment->b <= start_pos && segment->d >= end_pos)
          || (segment->d <= start_pos && segment->b >= end_pos)
      ){        
        wrapper->slabs[i].sorted_list[k] = segment,k++;
      }
      /* printf("%d\n", k);       */
    }
    qsort(wrapper->slabs[i].sorted_list,k,sizeof(struct line_segment*),comparison_linesegments_y);
    sum_total += k;
  }  
  return wrapper;
}




// Given a simple polygon p, is (x,y) in p?
bool point_location(struct polygon* p, float x, float y) {
  int counted =0;
  
  for(int i =0 ; i < p->length; i++) {
    struct line_segment* current = &p->segments[i];
    float line_y = y;
    float x_start = 0;
    float x_end = x;

    float m = (current->b- current->d)
      /(float)(current->a - current->c);        
    float c = current->d - m*current->c;    
    if (((y-c)/m <= x || (current->a == current->c && current->a < x) || (current->b == current->d && y == current->d) )&& (
          
          (current->b <= y && current->d > y) ||
          (current->d <= y && current->b > y)
        ))
    {
      /* printf("ID:%d\n", i); */
      counted +=1;
    }
  }
  /* printf("%d,%d,%d\n",counted,x,y); */
  return (counted%2);
}

int* find_nearest_point_on_polygon(struct polygon *p, int x, int y) {
  int* data = malloc(sizeof(int)*2);
  

  return data;
}

struct polygon* contour_polygon(struct polygon *p,float r) {

    struct polygon* c = malloc(sizeof(struct polygon));
    // Start by mallocing the array so that we can place the correct count there in the
    // first place
    c->segments = malloc(sizeof(struct line_segment)*p->length); 
    float* coords_x = alloca(sizeof(float)*p->length);
  float* coords_y = alloca(sizeof(float)*p->length);
  bool* internals = alloca(sizeof(bool)*p->length);
  for (int i  =0; i < p->length;i++ ) {    
    float abc[3];
    // Update this to use the modulo operator.. and therefore catch the tails.
    float a_x = p->segments[(i)%p->length].c,a_y = p->segments[i%p->length].d;
    float c_x = p->segments[(i+1)%p->length].c,c_y = p->segments[(i+1)%p->length].d;
    float b_x = p->segments[(i+2)%p->length].c,b_y = p->segments[(i+2)%p->length].d;
    
    
    abc[0] = sqrt(pow(a_x-b_x,2) + pow(a_y-b_y,2));
    abc[1] = sqrt(pow(c_x-b_x,2) + pow(c_y-b_y,2));
    abc[2] = sqrt(pow(a_x-c_x,2) + pow(a_y-c_y,2));
    float gamma = acos((abc[0] + abc[1] - abc[2])/(2.0*sqrt(abc[0])*sqrt(abc[1])));
    float alpha = acos((abc[1] + abc[2] - abc[0])/(2.0*sqrt(abc[1])*sqrt(abc[2])));
    float beta = acos((abc[2] + abc[0] - abc[1])/(2.0*sqrt(abc[2])*sqrt(abc[0])));
    float half_gamma = gamma/2;

    float interior_angle_a = M_PI - (half_gamma + alpha),
        interior_angle_b = M_PI - (half_gamma + beta);

    // normalise the distance vector between both a,c,b
    float  ac_x_norm = (c_x - a_x)/abc[2], cb_x_norm = (c_x - b_x)/abc[1],
        ac_y_norm = (c_y - a_y)/abc[2], cb_y_norm = (c_y - b_y)/abc[1];        

    // Find the corner points of,
    /* printf("\norigins1: %.2f,%.2f,%.2f,%.2f\n",a_x,a_y,b_x,b_y); */
    a_x = c_x + ac_x_norm,    
    a_y = c_y + ac_y_norm,
    b_x = c_x + cb_x_norm,
    b_y = c_y + cb_y_norm;
    
    float x = (a_x + b_x)/2, y = (a_y + b_y) /2;
    bool crossing_inside =point_location(p,x,y);    
    float mult = -1;
    if (!crossing_inside)
      mult = 1;

    internals[i%p->length] = crossing_inside;
    
    float dist_pos = sqrt(pow(x-c_x,2) + pow(y-c_y,2));    
    float x_dv = (c_x-(float)(x)),
        y_dv = (c_y-(float)(y));
    
    coords_x[(i+1)%p->length] = (float)c_x + (x_dv*mult*r)/dist_pos;
    coords_y[(i+1)%p->length] = (float)c_y + (y_dv*mult*r)/dist_pos;
    /* printf("Coords[%d]: %.2f,%.2f\n",(i+1)%p->length,coords_x[(i+1)%p->length], coords_y[(i+1)%p->length]); */
    if(dist_pos == 0){
        /* printf("Distpos\n"); */
        coords_x[(i+1)%p->length] = coords_x[(i)%p->length],
        coords_y[(i+1)%p->length] = coords_y[(i)%p->length];
    }
        
    /* printf("Coords: %.2f,%.2f\n",coords_x[(i+1)%p->length], coords_y[(i+1)%p->length]); */
    
  }
  
  // One now needs to update the data-sturcture of $c$ in order to induce the correct names
  // One starts via, memory assignment of the first element,
  
  for (int i = 1; i < p->length+1; i++) {      
      c->segments[i-1].c = coords_x[i-1];
      c->segments[i-1].d = coords_y[i-1];
      c->segments[i%p->length].a = coords_x[i-1];
      c->segments[i%p->length].b = coords_y[i-1];      
      bool in = point_location(p,(int)coords_x[(i)%p->length], (int)coords_y[(i)%p->length]);
      internals[i%p->length] = in;      
  }
  
  c->length = p->length;
  /* for (int i =0; i < p->length; i++ ) { */
  /*     printf("%d,%d,%d,%d\n",c->segments[i].a,c->segments[i].b,c->segments[i].c,c->segments[i].d); */
  /* } */
  /* fclose(k);   */
  return c;
}

// Returns true if the point sits in the two-set toolwidths for the polygon line placement
float * point_location_in_polygon_chain_struct_contours(
  struct polygon** s,int length, float x, float y) {
  // First one is the none hole
  // the remainder are holes  
  x = (float)x;
  y = (float)y;

  float* data = malloc(sizeof(float)*2);
  data[0] =x;
  data[1] =y;
#define l2norm(x,y,a,b) sqrt(pow(x-a,2) + pow(y-b,2))
  
  
  
  if(point_location(s[0],x,y) & !point_location(s[0]->tool1_contour,x,y)){
      float min = 5000;
  struct line_segment* nearest_segment = NULL;
  for (int i = 0; i < s[0]->length; i++) {
      struct line_segment* current = &s[0]->tool1_contour->segments[i];
      float x0=  x, y0 = y;
      float x1=  current->a, y1 = current->b;
      float x2=  current->c, y2 = current->d;
      float distance = (((y2 -y1)*x0 - (x2 - x1)*y0 + x2*y1-y2*x1)/sqrt(pow(y2-y1,2) + pow(x2 -x1,2)));
      if (distance < 0){ distance *= -1;}
      // See https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line#Line_defined_by_two_points
      
      if ((distance) < (min)) 
          min =distance, nearest_segment = current;
      
  }

  // Print out the reported positions, and output the correct design sets...
  FILE* u  = fopen("reported_positions.txt","a");
  fprintf(u,"%.2f,%.2f",x,y);
  
  
  /* printf("%.2f\n",min ); */
  // We know the distance, we can compute a normalised distance vector to each
  // triangle corner, then we can 
  float a_dist = sqrt(pow(x-nearest_segment->a,2) + pow(y-nearest_segment->b,2)),
    b_dist = sqrt(pow(x-nearest_segment->c,2) + pow(y-nearest_segment->d,2));

  float x_vect_dist_a = (nearest_segment->a - x)/a_dist,
      y_vect_dist_a = (nearest_segment->b -y )/a_dist, 
      x_vect_dist_b = (nearest_segment->c -x)/b_dist,
      y_vect_dist_b = (nearest_segment->d -y)/b_dist; 

  float point_k_x = x_vect_dist_a + x;
  float point_k_y = y_vect_dist_a + y;
  float point_j_x = x_vect_dist_b + x;
  float point_j_y = y_vect_dist_b + y;
    
  float middle_vector_x = (point_k_x + point_j_x)/2; 
  float middle_vector_y = (point_k_y + point_j_y)/2;
  float dist =  sqrt(pow(middle_vector_x -x ,2)  + pow(middle_vector_y - y,2));
  float middle_vector_x_norm = (middle_vector_x-x)/dist; 
  float middle_vector_y_norm = (middle_vector_y-y)/dist;
  
  data[0] = x + middle_vector_x_norm*min;
  data[1] = y + middle_vector_y_norm*min;
  fprintf(u,",%.2f,%.2f\n",data[0],data[1]);
  fclose(u);
  
  if (nearest_segment == NULL) exit(1);            
  }    
  
  for (int i = 1; i < length; i++)
      if(!point_location(s[i],x,y) & point_location(s[i]->tool1_contour,x,y)) {
    float min = 5000;
    struct line_segment* nearest_segment = NULL;
    for (int j = 0; j < s[i]->length; j++) {
        struct line_segment* current = &s[i]->tool1_contour->segments[j];
        float x0=  x, y0 = y;
        float x1=  current->a, y1 = current->b;
        float x2=  current->c, y2 = current->d;
        float distance = (((y2 -y1)*x0 - (x2 - x1)*y0 + x2*y1-y2*x1)/sqrt(pow(y2-y1,2) + pow(x2 -x1,2)));
        if (distance < 0){ distance *= -1;}
        // See https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line#Line_defined_by_two_points
      
        if ((distance) < (min)) 
            min =distance, nearest_segment = current;      
    }
    float a_dist = sqrt(pow(x-nearest_segment->a,2) + pow(y-nearest_segment->b,2)),
    b_dist = sqrt(pow(x-nearest_segment->c,2) + pow(y-nearest_segment->d,2));

  float x_vect_dist_a = (nearest_segment->a - x)/a_dist,
      y_vect_dist_a = (nearest_segment->b -y )/a_dist, 
      x_vect_dist_b = (nearest_segment->c -x)/b_dist,
      y_vect_dist_b = (nearest_segment->d -y)/b_dist; 

  float point_k_x = x_vect_dist_a + x;
  float point_k_y = y_vect_dist_a + y;
  float point_j_x = x_vect_dist_b + x;
  float point_j_y = y_vect_dist_b + y;
    
  float middle_vector_x = (point_k_x + point_j_x)/2; 
  float middle_vector_y = (point_k_y + point_j_y)/2;
  float dist =  sqrt(pow(middle_vector_x -x ,2)  + pow(middle_vector_y - y,2));
  float middle_vector_x_norm = (middle_vector_x-x)/dist; 
  float middle_vector_y_norm = (middle_vector_y-y)/dist;
  
  data[0] = x + middle_vector_x_norm*min;
  data[1] = y + middle_vector_y_norm*min;        
      }
   return data;
}



int main(int argc, char *argv[]) {
    remove("reported_positions.txt");
  float tool_width = 1; // The width of the tool which is being used. 
  char file_name[200];
  bool is_length = false;
  int maximum_length_of_polygon = 100;
  for(int i =0; i < argc; i ++) {
    /* printf("Variable:%d,%s\n", i,argv[i]); */
    if(is_length) maximum_length_of_polygon = atoi(argv[i]);    
    if (i == 1)
      if(strlen(argv[i]) < 199) strcpy(file_name,argv[i]);
      else printf("Error. Filename too long\n"),exit(1);
    else if (strcmp(argv[i],"-l"))
      is_length = true;
  }
  tool_width = 1; // The width of the tool which is being used. 
  /* printf("%d\n", maximum_length_of_polygon); */
  // File format 1St line is the polygon.
  FILE* f = fopen(file_name,"r");
  remove("contoured.txt");
  FILE* q = fopen("contoured.txt","w");
  bool flip = false;
  int floored_and_polygon_length = (int) maximum_length_of_polygon/tool_width;
  bool** matrix = malloc(sizeof(bool*)*(floored_and_polygon_length + 1));
  for (int i  =0; i < maximum_length_of_polygon ;i++ ) {
      matrix[i] = malloc(sizeof(bool)*(floored_and_polygon_length + 1));         
  }
  for (int i  =0; i < maximum_length_of_polygon ;i++ ) {
      for (int j  =0; j < maximum_length_of_polygon ;j++ ) {
          matrix[i][j]=1;
      }  
  }
// Note that we've arbitrarily set the maximum size of the number of shapes at 10
  struct polygon** polygons_of_shape = malloc(sizeof(struct polygon*)*10); 
  int polygon_shape_index = 0;
  while (!feof(f)){
    struct polygon* p  =
      build_polygon(f,maximum_length_of_polygon);
    
    if (p->length == 1) break;
    polygons_of_shape[polygon_shape_index] = p;
    polygon_shape_index +=1;
    int mult = 1;
    if(flip)
        mult = -1;
    
    struct polygon* c =contour_polygon(p,1.5*mult);
    p->tool1_contour = c;
    /* fprintf(q,"\n"); */
    for (int i = 0; i < c->length; i++) {
        fprintf(q,"%.2f,%.2f\n",c->segments[i].a,c->segments[i].b );
    }
    struct polygon* c2 =contour_polygon(p,2*1.5*mult);
    p->tool2_contour = c2;
    fprintf(q,"\n");
    struct polygon* c3 =contour_polygon(p,0.5*mult);
    
    for (int i = 0; i < c2->length; i++) {
      fprintf(q,"%.2f,%.2f\n",c2->segments[i].a,c2->segments[i].b );
    }

    for (float i = 0; i < maximum_length_of_polygon; i+=tool_width)
        for (float j = 0; j < maximum_length_of_polygon; j+=tool_width) {
            int i_unwrap = (int) i, j_unwrap = (int) j;
            if (!flip) matrix[i_unwrap][j_unwrap] = matrix[i_unwrap][j_unwrap] & point_location(c3,i+1,j+1);
            else matrix[i_unwrap][j_unwrap] = matrix[i_unwrap][j_unwrap] & !point_location(c3,i+1,j+1);                            
        }    
    flip = true;
    
  }
  fclose(q);
  fclose(f);
  FILE * u = fopen("lattice.txt","w");
  FILE * k = fopen("map.txt","w");
  for (int i = 0; i < maximum_length_of_polygon; i+=tool_width){ 
      for (int j = 0; j < maximum_length_of_polygon; j+=tool_width){ 
          if (matrix[i][j]) fprintf(u,"%d,%d\n",i+1,j+1);
          fprintf(k,"%c",matrix[i][j] == 1 ? ' ' : 'x');
      }
      fprintf(k,"\n");
  }
  fclose(k);  
  fclose(u);

  char builder_string[200];
  sprintf(builder_string,"./test_builder %d map.txt", maximum_length_of_polygon);
  system(builder_string);
  sprintf(builder_string,"./test %d 0 data.txt", maximum_length_of_polygon);
  system(builder_string);  
  char cmd[200];
  sprintf(cmd, "./test_builder %d map.txt",maximum_length_of_polygon);
  /* printf("%s\n",cmd ); */
  system(cmd);
  sprintf(cmd, "./test %d 0 data.txt",maximum_length_of_polygon);
  printf("%s\n",cmd );
  system(cmd);
  // 'points_parsed.txt' gives us a file..
  // 'info.txt' gives us management info, line one/two gives the rooted shared-point.
  char c[2]; bool b = false; int origin[2];
  origin[0] = 0, origin[1] = 0;
  f = fopen("info.txt","r");
  while(fgets(c, 2, f)) {
    if (c[0]=='\n') {      
            b= !b;
            if (b == 0) {
              break;
            }
            origin[b]=0;
        } else  origin[b] *= 10, origin[b] += c[0]-48;    
  }
  printf("%d,%d\n",origin[0],origin[1]);
  fclose(f);
  printf("First_polygon_length:%d\n",polygons_of_shape[0]->length );;
  k = fopen("points_parsed.txt","r");  
  int v[2]; v[0] =0; v[1] =0;
  FILE * d = fopen("new_point_set.txt","w");
  bool is_negative = false;
  while(fgets(c, 2, k)) {    
    if (c[0]=='\n' || c[0]==',') {
      if(is_negative){
        v[b] *= -1;
        is_negative =false;
      }
            b= !b;
            if (b == 0) {
              v[0] = 2*origin[1] - v[0];
              int i = v[1], j = v[0], a = origin[0],b = origin[1];
              v[1] = a + j - b;
              v[0] = b - (i-a);
              v[0] = 2*origin[0] - v[0];
              v[1] = 2*origin[1] - v[1];
              float k[2];
              k[0] = (float)v[0] + 1;
              k[1] = (float)v[1] + 1;
              float* l = point_location_in_polygon_chain_struct_contours(polygons_of_shape,polygon_shape_index,k[0],k[1]);
              
              k[0] = l[0], k[1]  =l[1];
              if ((l[0] != 0 && l[1] != 0) ) fprintf(d,"%.2f,%.2f\n",k[0],k[1]);
              
              v[0] = 0,v[1]= 0;              
            }
            v[b]=0;
    } else
      if (c[0] == '-') is_negative = true;
      else v[b] *= 10, v[b] += c[0]-48;
  }
  
  fclose(k);
  
  printf("Tool Length: %.2f\n",tool_width);
}

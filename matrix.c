#include <stdio.h>
#include <stdlib.h>

#define DIE(s,i) { \
	fprintf(stderr, "%s\n", s) ;  \
	exit(i) ; \
}

typedef struct fraction {
	int num ;
	int den ;
} FRAC ;

typedef struct matrix {
	int rows ;
	int cols ;
	FRAC *vals ;
} MATRIX ;

void read_file (FILE *f, MATRIX *m) {
	int i = 0 ;
	fscanf(f,"%dx%d", &(m->rows), &(m->cols)) ;

	m->vals = (FRAC*) calloc(sizeof(FRAC), m->rows*m->cols) ;
	while(fscanf(f,"%d/%d", &((m->vals[i]).num), &((m->vals[i]).den)) != EOF && i < (m->rows * m->cols)) 
		i++ ;
}

FRAC* get_val(MATRIX *m, int row, int col) {
	return &m->vals[row*m->cols + col] ;
}

float frac2float(FRAC *f) {
	return (float)f->num / f->den ;
}

void inline free_matrix(MATRIX *m) {
	free(m->vals) ;
}


int main(int argc, char **argv) {
	FILE *in ;
	MATRIX m ;
	int j, i ;
	if(argc != 2 ) 
		DIE("Missing parameter", 1) ;

	in = fopen(argv[1], "r") ;
	if(!in)
		DIE("Could not open file", 2) ;
	read_file(in, &m) ;
	fclose(in) ;
	printf("%d x %d\n", m.rows, m.cols) ;

	free_matrix(&m) ;
	
	return 0 ;
}

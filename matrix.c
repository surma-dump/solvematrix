#include <stdio.h>
#include <stdlib.h>

#define DIE(s,i) { \
	fprintf(stderr, "%s\n", s) ;  \
	exit(i) ; \
}

#define MIN(a,b) (a<b)?a:b

typedef int frac_t ;
typedef struct fraction {
	frac_t num ;
	frac_t den ;
} FRAC ;

typedef struct matrix {
	int rows ;
	int cols ;
	FRAC *vals ;
} MATRIX ;


float inline frac_to_float(FRAC *f) {
	return (float)f->num / f->den ;
}


frac_t gcd(frac_t a, frac_t b) {
	if(a < 0)
		a = -a ;
	if(b < 0)
		b = -b ;
	if(a == 0)
		return b ;
	while(b != 0) {
		if (a > b)
			a = a-b ;
		else
			b = b-a ;
	}
	return a ;

}

void frac_copy(FRAC *tar, FRAC *src) {
	tar->num = src->num ;
	tar->den = src->den ;
}

void frac_cancel(FRAC *tar, FRAC *n) {
	frac_t g = gcd(n->num, n->den) ;
	tar->num = n->num / g ;
	tar->den = n->den / g ;
}

void frac_div(FRAC *tar, FRAC *divisor, FRAC *dividend) {
	FRAC t ;
	t.num = divisor->num * dividend->den ;
	t.den = divisor->den * dividend->num ;
	frac_copy(tar,&t) ;
	frac_cancel(tar, tar) ;
}

void frac_mul(FRAC *tar, FRAC *mul1, FRAC *mul2) {
	FRAC t ;
	t.num = mul1->num * mul2->num ;
	t.den = mul1->den * mul2->den ;
	frac_copy(tar,&t) ;
	frac_cancel(tar, tar) ;
}

void frac_add(FRAC *tar, FRAC *sum1, FRAC *sum2) {
	tar->num = sum1->num * sum2->den + sum2->num * sum1->den ;
	tar->den = sum1->den * sum2->den ;
	frac_cancel(tar, tar) ;

}

void frac_negate(FRAC *tar, FRAC *n) {
	tar->num = -n->num ;
	tar->den = n->den ;
}

void frac_inverse(FRAC *tar, FRAC *n) {
	FRAC t ;
	t.num = n->den ;
	t.den = n->num ;
	frac_copy(tar,&t) ;
}

void read_file (FILE *f, MATRIX *m) {
	int i = 0 ;
	fscanf(f,"%dx%d", &(m->rows), &(m->cols)) ;

	m->vals = (FRAC*) calloc(sizeof(FRAC), m->rows*m->cols) ;
	while(fscanf(f,"%d/%d", &((m->vals[i]).num), &((m->vals[i]).den)) != EOF && i < (m->rows * m->cols)) 
		i++ ;
}

FRAC inline *matrix_get_val(MATRIX *m, int row, int col) {
	return &m->vals[row*m->cols + col] ;
}

void matrix_multiply_row(MATRIX *m, int row, FRAC *mul) {
	int i;
	for(i = 0; i < m->cols; i++) {
		FRAC *v = matrix_get_val(m, row, i) ;
		FRAC res ;
		frac_mul(&res, v, mul) ;
		frac_copy(v, &res) ;
	}
}

void matrix_add_row(MATRIX *m, int row, int add_row) {
	int i ;
	for(i = 0; i < m->cols ; i++) {
		FRAC *t = matrix_get_val(m, row, i) ;
		FRAC *a = matrix_get_val(m, add_row, i) ;
		FRAC f ;
		frac_add(&f, t, a) ;
		frac_copy(t, &f) ;
	}
}

void matrix_to_entity(MATRIX *m) {
	int depth, cur, maxdepth = MIN(m->cols, m->rows) ;
	for(depth = 0; depth < maxdepth; depth++) {
		for (cur = depth+1; cur < maxdepth; cur++) {
			FRAC *piv = matrix_get_val(m, depth, depth) ;
			FRAC *tz = matrix_get_val(m, cur, depth) ;
			FRAC fac ;
			if(tz->num != 0) {
				frac_div(&fac, piv, tz) ;
				frac_negate(&fac, &fac) ;
				matrix_multiply_row(m, cur, &fac) ;	
				matrix_add_row(m, cur, depth) ;

			}
		}
	}
	for(depth = maxdepth - 1; depth >= 0; depth--) {
		for (cur = depth-1; cur >= 0; cur--) {
			FRAC *piv = matrix_get_val(m, depth, depth) ;
			FRAC *tz = matrix_get_val(m, cur, depth) ;
			FRAC fac ;
			if(tz->num != 0) {
				frac_div(&fac, piv, tz) ;
				frac_negate(&fac, &fac) ;
				matrix_multiply_row(m, cur, &fac) ;	
				matrix_add_row(m, cur, depth) ;

			}
		}
	}
	for(depth = 0; depth < maxdepth; depth++) {
		FRAC t, *e = matrix_get_val(m, depth, depth) ;
		if (e->num != 0) {
			frac_inverse(&t,e) ;
			matrix_multiply_row(m, depth, &t) ;
		}
	}

}

void inline free_matrix(MATRIX *m) {
	free(m->vals) ;
}

void print_matrix(MATRIX *m) {
	int row, col ;
	for(row = 0; row < m->rows; row++) {
		for(col = 0; col < m->cols; col++) {
			FRAC *f = matrix_get_val(m, row, col) ;
			printf("(%5d/%-5d) ", f->num, f->den) ;
		}
		printf("\n");
	}
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

	print_matrix(&m) ;
	printf("========\n") ;
	matrix_to_entity(&m)  ;
	print_matrix(&m) ;

	free_matrix(&m) ;
	
	return 0 ;
}


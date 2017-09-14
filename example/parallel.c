typedef unsigned int size_t;
typedef void *__gnuc_va_list;
typedef unsigned char __u_char;
typedef unsigned short __u_short;
typedef unsigned int __u_int;
typedef unsigned long __u_long;
__extension__ typedef unsigned long long int __u_quad_t;
__extension__ typedef long long int __quad_t;
typedef signed char __int8_t;
typedef unsigned char __uint8_t;
typedef signed short int __int16_t;
typedef unsigned short int __uint16_t;
typedef signed int __int32_t;
typedef unsigned int __uint32_t;
__extension__ typedef signed long long int __int64_t;
__extension__ typedef unsigned long long int __uint64_t;
typedef __quad_t *__qaddr_t;
typedef __u_quad_t __dev_t;		 
typedef __u_int __uid_t;		 
typedef __u_int __gid_t;		 
typedef __u_long __ino_t;		 
typedef __u_int __mode_t;		 
typedef __u_int __nlink_t; 		 
typedef long int __off_t;		 
typedef __quad_t __loff_t;		 
typedef int __pid_t;			 
typedef int __ssize_t;			 
typedef long int __rlim_t;		 
typedef __quad_t __rlim64_t;		 
typedef __u_int __id_t;			 
typedef struct
  {
    int __val[2];
  } __fsid_t;				 
typedef int __daddr_t;			 
typedef char *__caddr_t;
typedef long int __time_t;
typedef long int __swblk_t;		 
typedef long int __clock_t;
typedef unsigned long int __fd_mask;
typedef struct
  {
    __fd_mask __fds_bits[1024  / (8 * sizeof (__fd_mask)) ];
  } __fd_set;
typedef int __key_t;
typedef unsigned short int __ipc_pid_t;
typedef long int __blkcnt_t;
typedef __quad_t __blkcnt64_t;
typedef __u_long __fsblkcnt_t;
typedef __u_quad_t __fsblkcnt64_t;
typedef __u_long __fsfilcnt_t;
typedef __u_quad_t __fsfilcnt64_t;
typedef __u_long __ino64_t;
typedef __loff_t __off64_t;
typedef long int __t_scalar_t;
typedef unsigned long int __t_uscalar_t;
typedef int __intptr_t;
typedef struct _IO_FILE FILE;
typedef long int wchar_t;
typedef unsigned int  wint_t;
typedef int _G_int16_t __attribute__ ((__mode__ (__HI__)));
typedef int _G_int32_t __attribute__ ((__mode__ (__SI__)));
typedef unsigned int _G_uint16_t __attribute__ ((__mode__ (__HI__)));
typedef unsigned int _G_uint32_t __attribute__ ((__mode__ (__SI__)));
struct _IO_jump_t;  struct _IO_FILE;
typedef void _IO_lock_t;
struct _IO_marker {
  struct _IO_marker *_next;
  struct _IO_FILE *_sbuf;
  int _pos;
};
struct _IO_FILE {
  int _flags;		 
  char* _IO_read_ptr;	 
  char* _IO_read_end;	 
  char* _IO_read_base;	 
  char* _IO_write_base;	 
  char* _IO_write_ptr;	 
  char* _IO_write_end;	 
  char* _IO_buf_base;	 
  char* _IO_buf_end;	 
  char *_IO_save_base;  
  char *_IO_backup_base;   
  char *_IO_save_end;  
  struct _IO_marker *_markers;
  struct _IO_FILE *_chain;
  int _fileno;
  int _blksize;
  __off_t   _old_offset;  
  unsigned short _cur_column;
  signed char _vtable_offset;
  char _shortbuf[1];
  _IO_lock_t *_lock;
  __off64_t   _offset;
  int _unused2[16];
};
typedef struct _IO_FILE _IO_FILE;
struct _IO_FILE_plus;
extern struct _IO_FILE_plus _IO_2_1_stdin_;
extern struct _IO_FILE_plus _IO_2_1_stdout_;
extern struct _IO_FILE_plus _IO_2_1_stderr_;
typedef __ssize_t __io_read_fn  (void *  __cookie, char *__buf,
				       size_t __nbytes)  ;
typedef __ssize_t __io_write_fn  (void *  __cookie, __const char *__buf,
				      size_t __n)  ;
typedef int __io_seek_fn  (void *  __cookie, __off_t   __pos, int __w)  ;
typedef int __io_close_fn  (void *  __cookie)  ;
extern int __underflow  (_IO_FILE *)    ;
extern int __uflow  (_IO_FILE *)    ;
extern int __overflow  (_IO_FILE *, int)    ;
extern int _IO_getc  (_IO_FILE *__fp)    ;
extern int _IO_putc  (int __c, _IO_FILE *__fp)    ;
extern int _IO_feof  (_IO_FILE *__fp)    ;
extern int _IO_ferror  (_IO_FILE *__fp)    ;
extern int _IO_peekc_locked  (_IO_FILE *__fp)    ;
extern void _IO_flockfile  (_IO_FILE *)    ;
extern void _IO_funlockfile  (_IO_FILE *)    ;
extern int _IO_ftrylockfile  (_IO_FILE *)    ;
extern int _IO_vfscanf  (_IO_FILE * __restrict, const char * __restrict,
			     __gnuc_va_list , int *__restrict)    ;
extern int _IO_vfprintf  (_IO_FILE *__restrict, const char *__restrict,
			      __gnuc_va_list )    ;
extern __ssize_t   _IO_padn  (_IO_FILE *, int, __ssize_t  )    ;
extern size_t   _IO_sgetn  (_IO_FILE *, void *, size_t  )    ;
extern __off64_t   _IO_seekoff  (_IO_FILE *, __off64_t  , int, int)    ;
extern __off64_t   _IO_seekpos  (_IO_FILE *, __off64_t  , int)    ;
extern void _IO_free_backup_area  (_IO_FILE *)    ;
typedef __off_t  fpos_t;
extern FILE *stdin;		 
extern FILE *stdout;		 
extern FILE *stderr;		 
extern int remove  (__const char *__filename)    ;
extern int rename  (__const char *__old, __const char *__new)    ;
extern FILE *tmpfile  (void)    ;
extern char *tmpnam  (char *__s)    ;
extern char *tmpnam_r  (char *__s)    ;
extern char *tempnam  (__const char *__dir, __const char *__pfx)    ;
extern int fclose  (FILE *__stream)    ;
extern int fflush  (FILE *__stream)    ;
extern int fflush_unlocked  (FILE *__stream)    ;
extern FILE *fopen  (__const char *__restrict __filename,
			 __const char *__restrict __modes)    ;
extern FILE *freopen  (__const char *__restrict __filename,
			   __const char *__restrict __modes,
			   FILE *__restrict __stream)    ;
extern FILE *fdopen  (int __fd, __const char *__modes)    ;
extern void setbuf  (FILE *__restrict __stream, char *__restrict __buf)    ;
extern int setvbuf  (FILE *__restrict __stream, char *__restrict __buf,
			 int __modes, size_t __n)    ;
extern void setbuffer  (FILE *__restrict __stream, char *__restrict __buf,
			    size_t __size)    ;
extern void setlinebuf  (FILE *__stream)    ;
extern int fprintf  (FILE *__restrict __stream,
			 __const char *__restrict __format, ...)    ;
extern int printf  (__const char *__restrict __format, ...)    ;
extern int sprintf  (char *__restrict __s,
			 __const char *__restrict __format, ...)    ;
extern int vfprintf  (FILE *__restrict __s,
			  __const char *__restrict __format,
			  __gnuc_va_list  __arg)    ;
extern int vprintf  (__const char *__restrict __format,
			 __gnuc_va_list  __arg)    ;
extern int vsprintf  (char *__restrict __s,
			  __const char *__restrict __format,
			  __gnuc_va_list  __arg)    ;
extern int snprintf  (char *__restrict __s, size_t __maxlen,
			  __const char *__restrict __format, ...)    
     __attribute__ ((__format__ (__printf__, 3, 4)));
extern int __vsnprintf  (char *__restrict __s, size_t __maxlen,
			     __const char *__restrict __format,
			     __gnuc_va_list  __arg)    
     __attribute__ ((__format__ (__printf__, 3, 0)));
extern int vsnprintf  (char *__restrict __s, size_t __maxlen,
			   __const char *__restrict __format,
			   __gnuc_va_list  __arg)    
     __attribute__ ((__format__ (__printf__, 3, 0)));
extern int fscanf  (FILE *__restrict __stream,
			__const char *__restrict __format, ...)    ;
extern int scanf  (__const char *__restrict __format, ...)    ;
extern int sscanf  (__const char *__restrict __s,
			__const char *__restrict __format, ...)    ;
extern int fgetc  (FILE *__stream)    ;
extern int getc  (FILE *__stream)    ;
extern int getchar  (void)    ;
extern int getc_unlocked  (FILE *__stream)    ;
extern int getchar_unlocked  (void)    ;
extern int fgetc_unlocked  (FILE *__stream)    ;
extern int fputc  (int __c, FILE *__stream)    ;
extern int putc  (int __c, FILE *__stream)    ;
extern int putchar  (int __c)    ;
extern int fputc_unlocked  (int __c, FILE *__stream)    ;
extern int putc_unlocked  (int __c, FILE *__stream)    ;
extern int putchar_unlocked  (int __c)    ;
extern int getw  (FILE *__stream)    ;
extern int putw  (int __w, FILE *__stream)    ;
extern char *fgets  (char *__restrict __s, int __n,
			 FILE *__restrict __stream)    ;
extern char *gets  (char *__s)    ;
extern int fputs  (__const char *__restrict __s,
		       FILE *__restrict __stream)    ;
extern int puts  (__const char *__s)    ;
extern int ungetc  (int __c, FILE *__stream)    ;
extern size_t fread  (void *__restrict __ptr, size_t __size,
			  size_t __n, FILE *__restrict __stream)    ;
extern size_t fwrite  (__const void *__restrict __ptr, size_t __size,
			   size_t __n, FILE *__restrict __s)    ;
extern size_t fread_unlocked  (void *__restrict __ptr, size_t __size,
				   size_t __n, FILE *__restrict __stream)    ;
extern size_t fwrite_unlocked  (__const void *__restrict __ptr,
				    size_t __size, size_t __n,
				    FILE *__restrict __stream)    ;
extern int fseek  (FILE *__stream, long int __off, int __whence)    ;
extern long int ftell  (FILE *__stream)    ;
extern void rewind  (FILE *__stream)    ;
typedef __off_t off_t;
extern int fgetpos  (FILE *__restrict __stream,
			 fpos_t *__restrict __pos)    ;
extern int fsetpos  (FILE *__stream, __const fpos_t *__pos)    ;
extern void clearerr  (FILE *__stream)    ;
extern int feof  (FILE *__stream)    ;
extern int ferror  (FILE *__stream)    ;
extern void clearerr_unlocked  (FILE *__stream)    ;
extern int feof_unlocked  (FILE *__stream)    ;
extern int ferror_unlocked  (FILE *__stream)    ;
extern void perror  (__const char *__s)    ;
extern int sys_nerr;
extern __const char *__const sys_errlist[];
extern int fileno  (FILE *__stream)    ;
extern int fileno_unlocked  (FILE *__stream)    ;
extern FILE *popen  (__const char *__command, __const char *__modes)    ;
extern int pclose  (FILE *__stream)    ;
extern char *ctermid  (char *__s)    ;
extern void flockfile  (FILE *__stream)    ;
extern int ftrylockfile  (FILE *__stream)    ;
extern void funlockfile  (FILE *__stream)    ;
typedef struct
  {
    int quot;			 
    int rem;			 
  } div_t;
typedef struct
  {
    long int quot;		 
    long int rem;		 
  } ldiv_t;
extern size_t __ctype_get_mb_cur_max  (void)    ;
extern double atof  (__const char *__nptr)    ;
extern int atoi  (__const char *__nptr)    ;
extern long int atol  (__const char *__nptr)    ;
__extension__ extern long long int atoll  (__const char *__nptr)    ;
extern double strtod  (__const char *__restrict __nptr,
			   char **__restrict __endptr)    ;
extern long int strtol  (__const char *__restrict __nptr,
			     char **__restrict __endptr, int __base)    ;
extern unsigned long int strtoul  (__const char *__restrict __nptr,
				       char **__restrict __endptr,
				       int __base)    ;
__extension__
extern long long int strtoq  (__const char *__restrict __nptr,
				  char **__restrict __endptr, int __base)    ;
__extension__
extern unsigned long long int strtouq  (__const char *__restrict __nptr,
					    char **__restrict __endptr,
					    int __base)    ;
__extension__
extern long long int strtoll  (__const char *__restrict __nptr,
				   char **__restrict __endptr, int __base)    ;
__extension__
extern unsigned long long int strtoull  (__const char *__restrict __nptr,
					     char **__restrict __endptr,
					     int __base)    ;
extern double __strtod_internal  (__const char *__restrict __nptr,
				      char **__restrict __endptr,
				      int __group)    ;
extern float __strtof_internal  (__const char *__restrict __nptr,
				     char **__restrict __endptr, int __group)    ;
extern long double  __strtold_internal  (__const char *
						__restrict __nptr,
						char **__restrict __endptr,
						int __group)    ;
extern long int __strtol_internal  (__const char *__restrict __nptr,
					char **__restrict __endptr,
					int __base, int __group)    ;
extern unsigned long int __strtoul_internal  (__const char *
						  __restrict __nptr,
						  char **__restrict __endptr,
						  int __base, int __group)    ;
__extension__
extern long long int __strtoll_internal  (__const char *__restrict __nptr,
					      char **__restrict __endptr,
					      int __base, int __group)    ;
__extension__
extern unsigned long long int __strtoull_internal  (__const char *
							__restrict __nptr,
							char **
							__restrict __endptr,
							int __base,
							int __group)    ;
extern char *l64a  (long int __n)    ;
extern long int a64l  (__const char *__s)    ;
typedef __u_char u_char;
typedef __u_short u_short;
typedef __u_int u_int;
typedef __u_long u_long;
typedef __quad_t quad_t;
typedef __u_quad_t u_quad_t;
typedef __fsid_t fsid_t;
typedef __loff_t loff_t;
typedef __ino_t ino_t;
typedef __dev_t dev_t;
typedef __gid_t gid_t;
typedef __mode_t mode_t;
typedef __nlink_t nlink_t;
typedef __uid_t uid_t;
typedef __pid_t pid_t;
typedef __id_t id_t;
typedef __ssize_t ssize_t;
typedef __daddr_t daddr_t;
typedef __caddr_t caddr_t;
typedef __key_t key_t;
typedef __time_t time_t;
typedef unsigned long int ulong;
typedef unsigned short int ushort;
typedef unsigned int uint;
typedef int int8_t __attribute__ ((__mode__ (  __QI__ ))) ;
typedef int int16_t __attribute__ ((__mode__ (  __HI__ ))) ;
typedef int int32_t __attribute__ ((__mode__ (  __SI__ ))) ;
typedef int int64_t __attribute__ ((__mode__ (  __DI__ ))) ;
typedef unsigned int u_int8_t __attribute__ ((__mode__ (  __QI__ ))) ;
typedef unsigned int u_int16_t __attribute__ ((__mode__ (  __HI__ ))) ;
typedef unsigned int u_int32_t __attribute__ ((__mode__ (  __SI__ ))) ;
typedef unsigned int u_int64_t __attribute__ ((__mode__ (  __DI__ ))) ;
typedef int register_t __attribute__ ((__mode__ (__word__)));
typedef int __sig_atomic_t;
typedef struct
  {
    unsigned long int __val[(1024 / (8 * sizeof (unsigned long int))) ];
  } __sigset_t;
struct timespec
  {
    long int tv_sec;		 
    long int tv_nsec;		 
  };
struct timeval;
typedef __fd_mask fd_mask;
typedef __fd_set fd_set;
extern int __select  (int __nfds, __fd_set *__readfds,
			  __fd_set *__writefds, __fd_set *__exceptfds,
			  struct timeval *__timeout)    ;
extern int select  (int __nfds, __fd_set *__readfds,
			__fd_set *__writefds, __fd_set *__exceptfds,
			struct timeval *__timeout)    ;
typedef __blkcnt_t blkcnt_t;	  
typedef __fsblkcnt_t fsblkcnt_t;  
typedef __fsfilcnt_t fsfilcnt_t;  
extern int32_t random  (void)    ;
extern void srandom  (unsigned int __seed)    ;
extern void *  initstate  (unsigned int __seed, void *  __statebuf,
			       size_t __statelen)    ;
extern void *  setstate  (void *  __statebuf)    ;
struct random_data
  {
    int32_t *fptr;		 
    int32_t *rptr;		 
    int32_t *state;		 
    int rand_type;		 
    int rand_deg;		 
    int rand_sep;		 
    int32_t *end_ptr;		 
  };
extern int random_r  (struct random_data *__restrict __buf,
			  int32_t *__restrict __result)    ;
extern int srandom_r  (unsigned int __seed, struct random_data *__buf)    ;
extern int initstate_r  (unsigned int __seed,
			     void *  __restrict __statebuf,
			     size_t __statelen,
			     struct random_data *__restrict __buf)    ;
extern int setstate_r  (void *  __restrict __statebuf,
			    struct random_data *__restrict __buf)    ;
extern int rand  (void)    ;
extern void srand  (unsigned int __seed)    ;
extern int rand_r  (unsigned int *__seed)    ;
extern double drand48  (void)    ;
extern double erand48  (unsigned short int __xsubi[3])    ;
extern long int lrand48  (void)    ;
extern long int nrand48  (unsigned short int __xsubi[3])    ;
extern long int mrand48  (void)    ;
extern long int jrand48  (unsigned short int __xsubi[3])    ;
extern void srand48  (long int __seedval)    ;
extern unsigned short int *seed48  (unsigned short int __seed16v[3])    ;
extern void lcong48  (unsigned short int __param[7])    ;
struct drand48_data
  {
    unsigned short int x[3];	 
    unsigned short int a[3];	 
    unsigned short int c;	 
    unsigned short int old_x[3];  
    int init;			 
  };
extern int drand48_r  (struct drand48_data *__restrict __buffer,
			   double *__restrict __result)    ;
extern int erand48_r  (unsigned short int __xsubi[3],
			   struct drand48_data *__restrict __buffer,
			   double *__restrict __result)    ;
extern int lrand48_r  (struct drand48_data *__restrict __buffer,
			   long int *__restrict __result)    ;
extern int nrand48_r  (unsigned short int __xsubi[3],
			   struct drand48_data *__restrict __buffer,
			   long int *__restrict __result)    ;
extern int mrand48_r  (struct drand48_data *__restrict __buffer,
			   long int *__restrict __result)    ;
extern int jrand48_r  (unsigned short int __xsubi[3],
			   struct drand48_data *__restrict __buffer,
			   long int *__restrict __result)    ;
extern int srand48_r  (long int __seedval, struct drand48_data *__buffer)    ;
extern int seed48_r  (unsigned short int __seed16v[3],
			  struct drand48_data *__buffer)    ;
extern int lcong48_r  (unsigned short int __param[7],
			   struct drand48_data *__buffer)    ;
extern void *  malloc  (size_t __size)    ;
extern void *  calloc  (size_t __nmemb, size_t __size)    ;
extern void *  realloc  (void *  __ptr, size_t __size)    ;
extern void free  (void *  __ptr)    ;
extern void cfree  (void *  __ptr)    ;
extern void *  alloca  (size_t __size)    ;
extern void *  valloc  (size_t __size)    ;
extern void abort  (void)     __attribute__ ((__noreturn__));
extern int atexit  (void (*__func) (void))    ;
extern int __on_exit  (void (*__func) (int __status, void *  __arg),
			   void *  __arg)    ;
extern int on_exit  (void (*__func) (int __status, void *  __arg),
			 void *  __arg)    ;
extern void exit  (int __status)     __attribute__ ((__noreturn__));
extern char *getenv  (__const char *__name)    ;
extern char *__secure_getenv  (__const char *__name)    ;
extern int putenv  (char *__string)    ;
extern int setenv  (__const char *__name, __const char *__value,
			int __replace)    ;
extern void unsetenv  (__const char *__name)    ;
extern int clearenv  (void)    ;
extern char *mktemp  (char *__template)    ;
extern int mkstemp  (char *__template)    ;
extern int system  (__const char *__command)    ;
extern char *realpath  (__const char *__restrict __name,
			    char *__restrict __resolved)    ;
typedef int (*__compar_fn_t)  (__const void * , __const void * )  ;
extern void *  bsearch  (__const void *  __key, __const void *  __base,
			       size_t __nmemb, size_t __size,
			       __compar_fn_t __compar)  ;
extern void qsort  (void *  __base, size_t __nmemb, size_t __size,
			  __compar_fn_t __compar)  ;
extern int abs  (int __x)     __attribute__ ((__const__));
extern long int labs  (long int __x)     __attribute__ ((__const__));
extern div_t div  (int __numer, int __denom)     __attribute__ ((__const__));
extern ldiv_t ldiv  (long int __numer, long int __denom)    
     __attribute__ ((__const__));
extern char *ecvt  (double __value, int __ndigit, int *__restrict __decpt,
			int *__restrict __sign)    ;
extern char *fcvt  (double __value, int __ndigit, int *__restrict __decpt,
			int *__restrict __sign)    ;
extern char *gcvt  (double __value, int __ndigit, char *__buf)    ;
extern char *qecvt  (long double  __value, int __ndigit,
			 int *__restrict __decpt, int *__restrict __sign)    ;
extern char *qfcvt  (long double  __value, int __ndigit,
			 int *__restrict __decpt, int *__restrict __sign)    ;
extern char *qgcvt  (long double  __value, int __ndigit, char *__buf)    ;
extern int ecvt_r  (double __value, int __ndigit, int *__restrict __decpt,
			int *__restrict __sign, char *__restrict __buf,
			size_t __len)    ;
extern int fcvt_r  (double __value, int __ndigit, int *__restrict __decpt,
			int *__restrict __sign, char *__restrict __buf,
			size_t __len)    ;
extern int qecvt_r  (long double  __value, int __ndigit,
			 int *__restrict __decpt, int *__restrict __sign,
			 char *__restrict __buf, size_t __len)    ;
extern int qfcvt_r  (long double  __value, int __ndigit,
			 int *__restrict __decpt, int *__restrict __sign,
			 char *__restrict __buf, size_t __len)    ;
extern int mblen  (__const char *__s, size_t __n)    ;
extern int mbtowc  (wchar_t *__restrict __pwc,
			__const char *__restrict __s, size_t __n)    ;
extern int wctomb  (char *__s, wchar_t __wchar)    ;
extern size_t mbstowcs  (wchar_t *__restrict  __pwcs,
			     __const char *__restrict __s, size_t __n)    ;
extern size_t wcstombs  (char *__restrict __s,
			     __const wchar_t *__restrict __pwcs, size_t __n)    ;
extern int rpmatch  (__const char *__response)    ;
void print_array (int array[], int size, int array_num);
float find_variance (float mean, float meansq);
float find_mean_sq (int array [], int size);
float find_mean (int array[], int size);
void dual_sort (int a1[], int size1, int a2[], int size2, int out[]);
void sort_array (int array[], int size);
float find_diff_in_arrays(int a1[], int size1, int a2[], int size2);
int find_max_diff (int array[], int size);
int count_frequency_of_digit (int array[], int size, int digit);
int get_input (int array[], char * fname);
void cow();
void frog();
int main (int argc, char * argv[])
{
  int array_1[10000], array_2[10000], dig_array1[11], dig_array2[11];
  int full_array[20000], dig_array_full[11];
  int i, num_in1, num_in2, max_diff1, max_diff2;
  float mean_val1, mean_sq1, variance1;
  float mean_val2, mean_sq2, variance2;
  float mean_val_d, mean_sq_d, variance_d;
  float ave_item_diff_pre, ave_item_diff_post;
  num_in1 = get_input (array_1, argv[1]);
  num_in2 = get_input (array_2, argv[2]);
  max_diff1 = find_max_diff(array_1, num_in1);
  max_diff2 = find_max_diff(array_2, num_in2);
  ave_item_diff_pre = find_diff_in_arrays (array_1, num_in1, array_2, num_in2);
  sort_array (array_1, num_in1);
  sort_array (array_2, num_in2);
  dual_sort (array_1, num_in1, array_2, num_in2, full_array);
  ave_item_diff_post = find_diff_in_arrays (array_1, num_in1, array_2, num_in2);
  mean_val1 = find_mean (array_1, num_in1);
  mean_val2 = find_mean (array_2, num_in2);
  mean_sq1 = find_mean_sq (array_1, num_in1);
  mean_sq2 = find_mean_sq (array_2, num_in2);
  variance1 = find_variance (mean_val1, mean_sq1);
  variance2 = find_variance (mean_val2, mean_sq2);
  print_array (array_1, num_in1, 1);
  printf("maximum distance: %d\nmean: %f\nsquared mean: %f\nvariance: %f\n", 
	max_diff1, mean_val1, mean_sq1, variance1);
  print_array (array_2, num_in2, 2);
  printf("maximum distance: %d\nmean: %f\nsquared mean: %f\nvariance: %f\n", 
	max_diff2, mean_val2, mean_sq2, variance2);
  print_array (full_array, num_in1+num_in2, 3);
  printf("Average item difference before sorting: %f\n", ave_item_diff_pre);
  printf("Average item difference after sorting:  %f\n", ave_item_diff_post);
  printf("Digit counts:\n");
  printf("Digit:  Array 1  Array 2  Array 3\n");
  for (i = 0; i<=9; i++)
  {
    dig_array1[i] = count_frequency_of_digit(array_1, num_in1, i);
    printf("%5d%9d", i, count_frequency_of_digit(array_1, num_in1, i));
    dig_array2[i] = count_frequency_of_digit(array_2, num_in2, i);
    printf("%9d", count_frequency_of_digit(array_2, num_in2, i));
    dig_array_full[i] = count_frequency_of_digit(array_2, num_in2, i);
    printf("%9d\n", count_frequency_of_digit(full_array, num_in1+num_in2, i));
  }
  mean_val_d = find_mean (dig_array_full, 10);
  mean_sq_d = find_mean_sq (dig_array_full, 10);
  variance_d = find_variance (mean_val_d, mean_sq_d);
  printf("mean: %f\nsquared mean: %f\nvariance: %f\n", 
	mean_val_d, mean_sq_d, variance_d);
}
void print_array (int array[], int size, int array_num)
{
  int i;
  printf("Array %d\nEntry  Value\n", array_num);
  for (i = 0; i < size; i++)
    printf("%4d: %6d\n", i, array[i]);
}
float find_variance (float mean, float meansq)
{
  return (meansq - mean * mean);
}
float find_mean_sq (int array[], int size)
{
 int i, sum = 0;
  for (i = 0; i < size; i++)
    sum += (array[i] * array[i]);
  return (float)sum/(float)size;
}
float find_mean (int array[], int size)
{
  int i, sum = 0;
  for (i = 0; i < size; i++)
    sum += array[i];
  return (float)sum/(float)size;
}
void dual_sort (int a1[], int size1, int a2[], int size2, int out[])
{
  int i=0, j=0, k=0;
  while ((i < size1) && (j < size2))
  {
    if (a1[i] <= a2[j])
      out[k++] = a1[i++];
    else
      out[k++] = a2[j++];
  }
  if (i < size1)
    for (;i<size1;i++)
      out[k++] = a1[i];
  else if (j < size2)
    for (;j<size2;j++)
      out[k++] = a2[j];
}
void sort_array (int array[], int size)
{
  int i, j, temp;
  for (i = 0; i < size-1; i++)
    for (j = i+1; j < size; j++)
      if (array[i] > array[j])
      {
        temp = array[i];
        array[i] = array[j];
        array[j] = temp;
      }
}
float find_diff_in_arrays(int a1[], int size1, int a2[], int size2)
{
  int i, min_size, diff, total_diff = 0;
  if (size1 <= size2)
    min_size = size1;
  else
    min_size = size2;
  for (i = 0; i < min_size; i++)
  {
    diff = a1[i] - a2[i];
    if (diff < 0)
      diff = -diff;
    total_diff += diff;      
  }
  return (float)total_diff/(float) min_size;
}
int find_max_diff (int array[], int size)
{
  int i, diff, max_diff = 0;
  for (i = 1; i < size; i++)
  {
    diff = array[i-1] - array[i];
    if (diff < 0)
      diff = -diff;
    if (diff > max_diff)
      max_diff = diff;
  }
  return max_diff;
}
int count_frequency_of_digit (int array[], int size, int digit)
{
  int i, count=0, number, dig;
  for (i=0;i<size;i++)
  {
    number = array[i];
    if ((number == 0) && (digit == 0))
      count++;
    while (number > 0)
    {
      dig = number % 10;
      if (dig == digit)
        count++;
      number /= 10;
    }
  }
  return count;
}
int get_input (int array[], char * fname)
{
  FILE * fin;
  int i = 0;
  if (!(fin = fopen (fname, "r")))
  {
    printf("File not found\n");
    return -1;
  }
  while (!feof (fin))
  {
    fscanf (fin, "%d", &array[i++]);
  }
  return i-1;   
}

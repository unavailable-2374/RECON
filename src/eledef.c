#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "msps.h"


#define IMG_CAP 500000

/* mmap-based MSP file for random line access without rewind */
static char *msp_mmap_base = NULL;
static size_t msp_mmap_size = 0;
static long *msp_line_offsets = NULL;
static int msp_line_count = 0;

static void msp_mmap_init(const char *filename) {
  int fd;
  struct stat st;
  char *p, *end;
  int cap = 65536, count = 0;

  fd = open(filename, O_RDONLY);
  if (fd < 0) { fprintf(stderr, "Cannot open %s for mmap\n", filename); exit(2); }
  fstat(fd, &st);
  msp_mmap_size = st.st_size;
  msp_mmap_base = (char *) mmap(NULL, msp_mmap_size, PROT_READ, MAP_PRIVATE, fd, 0);
  close(fd);
  if (msp_mmap_base == MAP_FAILED) { fprintf(stderr, "mmap failed for %s\n", filename); exit(2); }

  /* build line offset index */
  msp_line_offsets = (long *) malloc(cap * sizeof(long));
  msp_line_offsets[count++] = 0;
  p = msp_mmap_base;
  end = msp_mmap_base + msp_mmap_size;
  while (p < end) {
    if (*p == '\n' && p + 1 < end) {
      if (count >= cap) {
        cap *= 2;
        msp_line_offsets = (long *) realloc(msp_line_offsets, cap * sizeof(long));
      }
      msp_line_offsets[count++] = (p + 1) - msp_mmap_base;
    }
    p++;
  }
  msp_line_count = count;
}

/* Get a line by 0-based line number into a buffer */
static int msp_mmap_getline(int lineno, char *buf, int bufsize) {
  long start, end_off;
  int len;
  if (lineno < 0 || lineno >= msp_line_count) return -1;
  start = msp_line_offsets[lineno];
  if (lineno + 1 < msp_line_count) end_off = msp_line_offsets[lineno + 1];
  else end_off = msp_mmap_size;
  len = end_off - start;
  if (len >= bufsize) len = bufsize - 1;
  memcpy(buf, msp_mmap_base + start, len);
  buf[len] = '\0';
  return len;
}

static void msp_mmap_cleanup(void) {
  if (msp_mmap_base && msp_mmap_base != MAP_FAILED) munmap(msp_mmap_base, msp_mmap_size);
  if (msp_line_offsets) free(msp_line_offsets);
}

#define INCLUDE_IMAGE 	    if (method == 1) {cov=sing_cov(&ele_frag, &cur->to_image->frag, cutoff);}\
            else {cov=doub_cov(&ele_frag, &cur->to_image->frag, cutoff);}\
            if (cov) {\
              img_ct ++;\
	      if (cur->to_image->index%2) {\
		(*(all_mprot+cur->to_image->index/2))->se = (*ecp);\
	      } else {\
		(*(all_mprot+cur->to_image->index/2))->pe = (*ecp);\
	      }\
	      fprintf(img_prot, "%d %d\n", (*ecp), cur->to_image->index);\
              /* fflush removed - using large buffer */\
	      if (!cur->next) tail = prev;\
	      if (prev) prev->next = cur->next;\
	      else remain = cur->next;\
	      if (ele_frag.rb < cur->to_image->frag.rb) {\
		ele_frag.rb = cur->to_image->frag.rb;\
		free(cur->to_image);\
		free(cur);\
		cur = remain;\
		prev = NULL;\
	      } else {\
		free(cur->to_image);\
		free(cur);\
		if (prev) cur = prev->next;\
		else cur = remain;\
	      }\
	    } else {\
	      prev = cur;\
	      cur = cur->next;\
	    }

#define ADD_EP      ep_tmp = (EPROT_t *) malloc(sizeof(EPROT_t));\
      ep_tmp->flag = 0;\
      ep_tmp->index = (*ecp);\
      ep_tmp->img_no = img_ct;\
      ep_tmp->frag = ele_frag;\
      ep_tmp->next = NULL;\
      if (*all_epp) ep_tail->next = ep_tmp;\
      else *all_epp = ep_tmp;\
      ep_tail = ep_tmp;\
      fprintf(all_ele, "%d %s %d %d\n", (*ecp), ele_frag.seq_name, ele_frag.lb, ele_frag.rb)

#define RITE_THING	ritetime = 0;\
	(*ecp) ++;\
        img_ct = 1;\
	ele_frag = cur->to_image->frag;\
	if (cur->to_image->index%2) {\
	  (*(all_mprot+cur->to_image->index/2))->se = (*ecp);\
	} else {\
	  (*(all_mprot+cur->to_image->index/2))->pe = (*ecp);\
	}\
	fprintf(img_prot, "%d %d\n", (*ecp), cur->to_image->index);\
        /* fflush removed - using large buffer */\
	remain = cur->next;\
	free(cur->to_image);\
	free(cur);\
	prev = NULL;\
	cur = remain

#define DO_IT	  if (ele_frag.seq_name == cur->to_image->frag.seq_name /*!strncmp(ele_frag.seq_name, cur->to_image->frag.seq_name, NAME_LEN)*/ && ele_frag.rb - cur->to_image->frag.lb > 10) {\
	    INCLUDE_IMAGE;\
	  } else {\
	    ADD_EP;\
	    ritetime = 1;\
	    break;\
	  }

#define BACK_UP	 imgp = (IMAGE_t *) malloc(sizeof(IMAGE_t));\
	*imgp = img;\
	imgp->img_data_node = NULL;\
	tmp = (IMG_DATA_t *) malloc(sizeof(IMG_DATA_t));\
	tmp->to_image = imgp;\
	tmp->prev = NULL;\
	tmp->next = NULL;\
	if (!remain) {\
	  remain = tmp;\
	} else {\
	  tail->next = tmp;\
	}\
	tail = tmp

#define DUMBBELL      img_charge(iprot_shadow, iprot_ct, msp_file);\
      for (i=0; i<iprot_ct; i++) {\
	if ((*(all_iprot+i))->index%2) {\
	  partner_index = (*(all_mprot+(*(all_iprot+i))->index/2))->pe;\
	} else {\
	  partner_index = (*(all_mprot+(*(all_iprot+i))->index/2))->se;\
	}\
	if ((*(all_iprot+i))->to_msp->score) {\
	  if (!(*(ep_array+((*(all_iprot+i))->ele_index-1)))->flag) {\
	    sprintf(ele_name, "ele_def_res/e%d", (*(all_iprot+i))->ele_index);\
	    freopen(ele_name, "w", all_ele);\
	    fprintf(all_ele, "index %d\n", (*(all_iprot+i))->ele_index);\
	    fprintf(all_ele, "frag %s %d %d\n", (*(ep_array+((*(all_iprot+i))->ele_index-1)))->frag.seq_name, (*(ep_array+((*(all_iprot+i))->ele_index-1)))->frag.lb, (*(ep_array+((*(all_iprot+i))->ele_index-1)))->frag.rb);\
            fprintf(all_ele, "img_no %d\n", (*(ep_array+((*(all_iprot+i))->ele_index-1)))->img_no);\
            fprintf(size_list, "%d %d\n", (*(all_iprot+i))->ele_index, (*(ep_array+((*(all_iprot+i))->ele_index-1)))->img_no);\
	    (*(ep_array+((*(all_iprot+i))->ele_index-1)))->flag = 1;\
	  }\
	  if (partner_index != (*(all_iprot+i))->ele_index) {\
	    if ((*(all_iprot+i))->index%2) fprintf(all_ele, "msp %d s %d %.1f %d %d %s %d %d %d %s %d %d\n", (*(all_iprot+i))->index, (*(all_iprot+i))->to_msp->score, (*(all_iprot+i))->to_msp->iden, (*(all_iprot+i))->to_msp->direction, partner_index, (*(all_iprot+i))->to_msp->query.frag.seq_name, (*(all_iprot+i))->to_msp->query.frag.lb, (*(all_iprot+i))->to_msp->query.frag.rb, (*(all_iprot+i))->ele_index, (*(all_iprot+i))->to_msp->sbjct.frag.seq_name, (*(all_iprot+i))->to_msp->sbjct.frag.lb, (*(all_iprot+i))->to_msp->sbjct.frag.rb);\
	    else fprintf(all_ele, "msp %d s %d %.1f %d %d %s %d %d %d %s %d %d\n", (*(all_iprot+i))->index, (*(all_iprot+i))->to_msp->score, (*(all_iprot+i))->to_msp->iden, (*(all_iprot+i))->to_msp->direction, (*(all_iprot+i))->ele_index, (*(all_iprot+i))->to_msp->query.frag.seq_name, (*(all_iprot+i))->to_msp->query.frag.lb, (*(all_iprot+i))->to_msp->query.frag.rb, partner_index, (*(all_iprot+i))->to_msp->sbjct.frag.seq_name, (*(all_iprot+i))->to_msp->sbjct.frag.lb, (*(all_iprot+i))->to_msp->sbjct.frag.rb);\
	  } else {\
	    if ((*(all_iprot+i))->index%2) fprintf(all_ele, "msp %d s %d %.1f %d %d %s %d %d %d %s %d %d\n", (*(all_iprot+i))->index, (*(all_iprot+i))->to_msp->score, (*(all_iprot+i))->to_msp->iden, (*(all_iprot+i))->to_msp->direction, partner_index, (*(all_iprot+i))->to_msp->query.frag.seq_name, (*(all_iprot+i))->to_msp->query.frag.lb, (*(all_iprot+i))->to_msp->query.frag.rb, (*(all_iprot+i))->ele_index, (*(all_iprot+i))->to_msp->sbjct.frag.seq_name, (*(all_iprot+i))->to_msp->sbjct.frag.lb, (*(all_iprot+i))->to_msp->sbjct.frag.rb);\
          }\
	}\
      }



typedef struct msp_prototype {
  int pe, se;
} MPROT_t;

typedef struct img_prototype {
  int index, ele_index;
  MSP_t *to_msp;
} IPROT_t;

typedef struct ele_prototype {
  short flag;
  int index, img_no;
  FRAG_t frag;
  struct ele_prototype *next;
} EPROT_t;


void ele_def(int, FILE *, float, EPROT_t **, int *, MPROT_t **);
void img_charge(IPROT_t **, int, FILE *);
int index_cmp(const void *, const void *);


FILE *msp_no, *all_ele, *img_prot, *ele_no, *err, *size_list;


int main (int argc, char *argv[]) {
  int ele_ct=0, msp_ct, i, method;
  float cutoff;
  char line[150], *m1="single", *m2="double";
  MPROT_t **all_mprot;
  EPROT_t *all_ep=NULL, **ep_array, *ep_tmp;

  IPROT_t **all_iprot, **iprot_shadow;
  int iprot_ct=0, partner_index;
  char ele_name[50]; /*name of element used as name of the ele file*/

  FILE *frags, *seq_list, *msp_file;

  /* processing command line */
  if (argc == 1) {
    printf("usage: ele_def seq_list msp_file method cut_off\nfor method, choose 'single' or 'double'\ncutoff is optional\n");
    exit(1);
  }

  seq_list = fopen(argv[1], "r");
  if (!seq_list) {
    printf("Input file for sequence name list %s not found.  Exit.\n", argv[1]);
    exit(2);
  }
  GetSeqNames(seq_list);

  msp_file = fopen(argv[2], "r");
  if (!msp_file) {
    printf("Input file of MSPs %s not found.  Exit.\n", argv[2]);
    exit(2);
  }
  msp_mmap_init(argv[2]);  /* mmap the MSP file for random line access */

  if (!strcmp(argv[3], m1)) {method = 1;}
  else if (!strcmp(argv[3], m2)) {method = 2;}
  else {
    printf("please choose single or double to indicate the method to evaluate overlap.\n");
    printf("%s is not a valid option.  Exit.\n", argv[3]);
    exit(2);
  }

  if (argc > 4) cutoff = atof(argv[4]);
  else {
    if (method == 1) {cutoff = 0.5;}
    else {cutoff = 0.9;}
  }

  /* opening files */
  if (!(frags = fopen("images/images_sorted", "r"))) {
    printf("Can not open the fragment list file, exiting.\n");
    exit(1);
  }
  if (!(msp_no = fopen("summary/ori_msp_no", "r"))) {
    printf("Can not open msp_no, exiting.\n");
    exit(1);
  }
  err = fopen("ele_def_res/errors", "w");
  all_ele = fopen("summary/naive_eles", "w");
  img_prot = fopen("ele_def_res/img_prot", "w");
  setvbuf(img_prot, NULL, _IOFBF, 65536);  /* 64K buffer instead of per-record fflush */
  ele_no = fopen("summary/naive_ele_no", "w");
  size_list = fopen("ele_def_res/size_list", "w");

  while(fgets(line, 100, msp_no)) {
    msp_ct = atoi(line);
  }

  all_mprot = (MPROT_t **) malloc(msp_ct*sizeof(MPROT_t *));
  for (i=0; i<msp_ct; i++) {
    *(all_mprot+i) = (MPROT_t *) malloc(sizeof(MPROT_t));
    (*(all_mprot+i))->pe = 0;
    (*(all_mprot+i))->se = 0;
  }

  ele_def(method, frags, cutoff, &all_ep, &ele_ct, all_mprot);

  /*for (i=0; i<msp_ct; i++) {
    if ((*(all_mprot+i))->pe > ele_ct || (*(all_mprot+i))->se > ele_ct) {
      printf("not OK\n");
      exit(1);
    }
  }*/

  fclose(ele_no);
  fclose(img_prot);
  fclose(msp_no);
  fclose(frags);
  img_prot = fopen("ele_def_res/img_prot", "r");

  ep_array = (EPROT_t **) malloc(ele_ct*sizeof(EPROT_t *));
  ep_tmp = all_ep;
  i = 0;
  while (ep_tmp) {
    *(ep_array+i) = ep_tmp;
    i ++;
    ep_tmp = ep_tmp->next;
  }

  all_iprot = (IPROT_t **) malloc(IMG_CAP*sizeof(IPROT_t *));
  iprot_shadow = (IPROT_t **) malloc(IMG_CAP*sizeof(IPROT_t *));
  for (i=0; i<IMG_CAP; i++) {
    *(all_iprot+i) = (IPROT_t *) malloc(sizeof(IPROT_t));
    (*(all_iprot+i))->to_msp = (MSP_t *) malloc(sizeof(MSP_t));
    *(iprot_shadow+i) = *(all_iprot+i);
  }

  while (fgets(line, 100, img_prot)) {
    sscanf(line, "%d %d\n", &(*(all_iprot+iprot_ct))->ele_index, &(*(all_iprot+iprot_ct))->index);
    iprot_ct ++;
    if (iprot_ct == IMG_CAP) {
      DUMBBELL;
      iprot_ct = 0;
    }
  }

  if (iprot_ct) {
    for (i=0; i<iprot_ct; i++) {*(iprot_shadow+i) = *(all_iprot+i);}
    DUMBBELL;
  }
  /* cleaning up */
  for (i=0; i<IMG_CAP; i++) {
    free((*(all_iprot+i))->to_msp);
    free(*(all_iprot+i));
  }
  free(all_iprot);
  free(iprot_shadow);

  for (i=0; i<msp_ct; i++) {
    free(*(all_mprot+i));
  }
  free(all_mprot);

  for (i=0; i<ele_ct; i++) {
    free(*(ep_array+i));
  }
  free(ep_array);

  msp_mmap_cleanup();
  exit(0);
}


void ele_def(int method, FILE *frags, float cutoff, EPROT_t **all_epp, int *ecp, MPROT_t **all_mprot) {
  int i, img_ct, cov;
  char line[100];
  FRAG_t ele_frag;
  char fragname[NAME_LEN];
  int pos;
  IMAGE_t img, *imgp;
  IMG_DATA_t *cur, *prev=NULL, *remain=NULL, *tail, *tmp;
  short ritetime;
  EPROT_t *ep_tail, *ep_tmp;
  /*short to_follow, to_finish=1;*/

  ritetime = 1; /* ritetime marks the right time to start a new element */
  while (fgets(line, 100, frags)) {
    sscanf(line, "%d %*d %s %d %d\n", &img.index, fragname, &img.frag.lb, &img.frag.rb);
    pos = GetSeqIndex(0, seq_no-1, fragname);
    img.frag.seq_name = *(seq_names+pos);

    if (ritetime) { /* starting a new element */
      ritetime = 0;
      (*ecp) ++;
      img_ct = 1;
      ele_frag = img.frag; /* start the definition of the element */
      if (img.index%2) {
	(*(all_mprot+img.index/2))->se = (*ecp);
      } else {
	(*(all_mprot+img.index/2))->pe = (*ecp);
      }
      fprintf(img_prot, "%d %d\n", (*ecp), img.index);

      continue;
    }
    if (ele_frag.seq_name == img.frag.seq_name /*!strncmp(ele_frag.seq_name, img.frag.seq_name, NAME_LEN)*/ && ele_frag.rb - img.frag.lb > 10) { /* checking possible images */
      if (method == 1) {cov = sing_cov(&ele_frag, &img.frag, cutoff);}
      else {cov = doub_cov(&ele_frag, &img.frag, cutoff);}
      if (cov) { /* a good image */
	/* include the current frag in the current element*/
	img_ct ++;
	if (img.index%2) {
	  (*(all_mprot+img.index/2))->se = (*ecp);
	} else {
	  (*(all_mprot+img.index/2))->pe = (*ecp);
	}
	fprintf(img_prot, "%d %d\n", (*ecp), img.index);

     	/* updating definition of the element if necessary */
	/* notice that it is a bit complicated than above */
	if (ele_frag.rb < img.frag.rb) { 
	  ele_frag.rb = img.frag.rb;
	  /* time to go back and check if previously unqualified images are now good to be participated to the updated element */
	  cur = remain;
	  prev = NULL;
	  while (cur) {
	    INCLUDE_IMAGE;
	  }
	}
      } else { /* current image not good for the current ele, keep it in the remaining list and move on */
	/*printf("1st backing up\n");*/
	BACK_UP;
      }
    } else { /* time to finish defining of ele_tmp */
      /* output the element */
      ADD_EP;
      /*printf("2nd backing up\n");*/
      BACK_UP;
      /* start a new element, finish the remain list */
      ritetime = 1;
      /* if (remain) printf("2nd look back\n");
	 else printf("no need for 2nd look back\n"); */
      while (remain && ritetime) {
	cur = remain;
	RITE_THING;
	while (cur) {
	  DO_IT;
	}
      }
    }
  }

  ADD_EP;
  if (remain) ritetime = 1;

  /* add the last element to the list of elements */
  /* if (remain) printf("finishing\n");
     else printf("no need to finish\n"); */
  while (remain) {
    cur = remain;
    prev = NULL;

    while (cur) {
      if (ritetime) {
	RITE_THING;
	continue;	
      }
      DO_IT;
    }
    if (!cur) {
      ADD_EP;
      ritetime = 1;
    }
  }

  fprintf(ele_no, "%d\n", (*ecp));
}





void img_charge(IPROT_t **shadow, int ct, FILE *input) {
  int i, pos;
  char line[200];
  int scan_flag;
  MSP_t msp;

  qsort(shadow, ct, sizeof(IPROT_t *), index_cmp);

  /* Use mmap random access: sorted shadow entries point to specific line numbers */
  for (i = 0; i < ct; ) {
    pos = (*(shadow+i))->index / 2;
    if (msp_mmap_getline(pos, line, sizeof(line)) < 0) {
      (*(shadow+i))->to_msp->score = 0;
      fprintf(err, "Line %d out of range for image %d\n", pos, (*(shadow+i))->index);
      i++;
      continue;
    }
    scan_flag = scan_msp(&msp, line);
    if (scan_flag) {
      (*(shadow+i))->to_msp->score = 0;
      fprintf(err, "Wrong format in the MSP file line %d for image %d\n", pos, (*(shadow+i))->index);
    } else {
      *((*(shadow+i))->to_msp) = msp;
    }
    i++;
    /* check if next image is from the same MSP line */
    if (i < ct && pos == (*(shadow+i))->index / 2) {
      if (scan_flag) {
        (*(shadow+i))->to_msp->score = 0;
        fprintf(err, "Wrong format in the MSP file line %d for image %d\n", pos, (*(shadow+i))->index);
      } else {
        *((*(shadow+i))->to_msp) = msp;
      }
      i++;
    }
  }
}





int index_cmp(const void *i1, const void *i2) {
  return (*((IPROT_t **) i1))->index - (*((IPROT_t **) i2))->index;
}

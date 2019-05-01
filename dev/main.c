#include <stdio.h>
#include <jpeglib.h>
#include <setjmp.h>

const char* data = "XYRNDOK!";
const size_t kDataSize = 8;

enum InteractionType {
  kModify,
  kRead
};

GLOBAL(void)
write_DCT_coef(const char* filename, struct jpeg_decompress_struct* in_cinfo, jvirt_barray_ptr* coeffs_array);
GLOBAL(void)
open_DCT_coefs(const char* filename, enum InteractionType interaction_type);

struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */

  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr* my_error_ptr;

METHODDEF(void)
my_error_exit (j_common_ptr cinfo) {
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  (*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}

GLOBAL(void)
open_DCT_coefs(const char* filename, enum InteractionType interaction_type) {
  struct jpeg_decompress_struct cinfo;
  struct my_error_mgr jerr;
  FILE* infile;
  JSAMPARRAY buffer;
  jvirt_barray_ptr* coeffs_array;

  if ((infile = fopen(filename, "rb")) == NULL) {
    fprintf(stderr, "can't open %s\n", filename);
    return;
  }

  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = my_error_exit;

  if (setjmp(jerr.setjmp_buffer)) {
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    return;
  }

  jpeg_create_decompress(&cinfo);
  jpeg_stdio_src(&cinfo, infile);
  jpeg_read_header(&cinfo, FALSE);
  coeffs_array = jpeg_read_coefficients(&cinfo);

  JBLOCKARRAY rowPtrs[MAX_COMPONENTS];

  rowPtrs[0] = ((&cinfo)->mem->access_virt_barray)((j_common_ptr) &cinfo, coeffs_array[0], 0, (JDIMENSION) 1, FALSE);

  for (int i = 0; i < kDataSize; i++) {

    interaction_type == kModify ? rowPtrs[0][0][0][i] = data[i] : printf("%c", rowPtrs[0][0][0][i]);
  }

  if(kModify == interaction_type) {
    write_DCT_coef(filename, &cinfo, coeffs_array);
  }
}

GLOBAL(void)
write_DCT_coef(const char* filename, struct jpeg_decompress_struct* in_cinfo, jvirt_barray_ptr* coeffs_array) {
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  FILE* infile;

  if((infile = fopen(filename, "wb")) == NULL) {
    fprintf(stderr, "can't open %s\n", filename);
    return;
  }

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);
  jpeg_stdio_dest(&cinfo, infile);
  jpeg_copy_critical_parameters(in_cinfo, &cinfo);
  jpeg_write_coefficients(&cinfo, coeffs_array);

  jpeg_finish_compress( &cinfo );
  jpeg_destroy_compress( &cinfo );
  fclose( infile );
}

int main() {
  const char* filename = "1.jpg";
  open_DCT_coefs(filename, kModify);
  open_DCT_coefs(filename, kRead);
  return 0;
}
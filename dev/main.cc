#include <stdio.h>
#include <jpeglib.h>
#include <setjmp.h>
#include <vector>
#include <bitset>
#include <iostream>

const char* payload = "Lorem ipsum dolor sit amet, consectetur adipiscing elit posuere.!";
const size_t kBitsCount = 8;
const size_t kPayloadSize = 64;
const size_t kDataSize = kBitsCount * kPayloadSize;
const unsigned int kPatternRead = 2;
const unsigned int kPatternWrite = 3;

enum InteractionType {
  kModify,
  kRead
};

void write_DCT_coef(const char* filename, struct jpeg_decompress_struct* in_cinfo, jvirt_barray_ptr* coeffs_array);
void open_DCT_coefs(const char* filename, enum InteractionType interaction_type);

struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */

  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr* my_error_ptr;

void my_error_exit (j_common_ptr cinfo) {
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  (*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}

void fill_write_data(std::vector<int>& data) {
  for(int index = 0; index < kPayloadSize; index++) {
    std::bitset<kBitsCount> bitset(static_cast<int>(payload[index]));
    for(int bit_index = kBitsCount - 1; bit_index >= 0; bit_index--) {
      data.push_back(bitset[bit_index]);
    }
  }
}

std::string bits_to_string(std::vector<int>& bits) {
  std::string output;

  int bits_index = 0;
  for(int index = 0; index < kPayloadSize; index++) {
    std::bitset<kBitsCount> bitset;
    for(int bit_index = kBitsCount - 1; bit_index >= 0; bit_index--) {
      bitset[bit_index] = bits[bits_index++];
    }

    output += static_cast<char>(bitset.to_ulong());
  }

  return output;
}

void open_DCT_coefs(const char* filename, InteractionType interaction_type, std::vector<int>& data) {
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

  int data_index = 0;

  for (JDIMENSION compNum = 0; compNum < cinfo.num_components; compNum++) {
    size_t blockRowSize = (size_t) sizeof(JCOEF) * DCTSIZE2 * cinfo.comp_info[compNum].width_in_blocks;
    for (JDIMENSION rowNum = 0; rowNum < cinfo.comp_info[compNum].height_in_blocks; rowNum++) {
      rowPtrs[compNum] = ((&cinfo)->mem->access_virt_barray)((j_common_ptr) &cinfo, coeffs_array[compNum],rowNum, (JDIMENSION) 1, FALSE);
      for (JDIMENSION blockNum = 0; blockNum < cinfo.comp_info[compNum].width_in_blocks; blockNum++) {
        for (JDIMENSION i = 0; i < DCTSIZE2; i++) {
          int digit = rowPtrs[compNum][0][blockNum][i];

          if(kModify == interaction_type) {

            if(data_index == data.size()) {
              write_DCT_coef(filename, &cinfo, coeffs_array);
              return;
            }

            if(digit >= kPatternRead) {

              if(data[data_index] == 0x00) {
                rowPtrs[compNum][0][blockNum][i] &= ~0x01;
              } else {
                rowPtrs[compNum][0][blockNum][i] |= data[data_index];
              }

              data_index++;
            }

          } else {

            if(digit >= kPatternRead) {

              if(data.size() == kDataSize) return;

              if(rowPtrs[compNum][0][blockNum][i] & 0x01) {
                data.push_back(0x01);
              } else {
                data.push_back(0x00);
              }

            }

          }

        }
      }
    }
  }

}

void write_DCT_coef(const char* filename, struct jpeg_decompress_struct* in_cinfo, jvirt_barray_ptr* coeffs_array) {
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
  std::vector<int> write_bits;
  std::vector<int> read_bits;
  const char* filename = "1.jpg";

  fill_write_data(write_bits);
  open_DCT_coefs(filename, kModify, write_bits);
  open_DCT_coefs(filename, kRead, read_bits);

  std::cout << bits_to_string(read_bits) << std::endl;

  return 0;
}
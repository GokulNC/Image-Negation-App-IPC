#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <assert.h>

struct Image {
  unsigned char* data;
  int width, height, channel;
};

typedef struct Image* Image;

Image readImage(char *filename, int numChannels) {
  Image img = malloc(sizeof(struct Image));
  /* reading image from disk as greyscale */
  img->data = stbi_load(filename, &(img->width), &(img->height), &(img->channel), numChannels);
  return img;
}

int writeImage(Image img, char *filename) {
  /* stroring the image on disk */
  stbi_write_png(filename, img->width, img->height, img->channel, img->data, img->width * img->channel);
  return 0;
}

void freeImage(Image img) {
  stbi_image_free( img->data );
  free(img);
}

void negateImage(Image img) {
  for (int i = 0; i < img->height * img->width * img->channel; ++i)
      img->data[i] = 255 - img->data[i];
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/archive.h"
#include "../include/utils.h"

#define MAX_FILES 32
#define MAX_TOTAL_SIZE (200L * 1024 * 1024) //200 MB
#define META_BUF_SIZE 16384 //16 KB yeterli

void archive_files(char *input_files[], int count, const char *output_file){
  long total_size = 0;
  char metadata[META_BUF_SIZE] = ""; //32 dosyanın bilgisi için
  char temp_buf[512];
  metadata[0] = '\0';
  
  //dosya doğrulama ve metadata hazırlığı
  for(int i = 0; i < count; i++){
  
    if(!is_text_file(input_files[i])){
      printf("%s Giriş dosyasının formatı uyumsuzdur!\n", input_files[i]);
      exit(0); //başarılı çıkış
      
    }
  
  long size = get_file_size(input_files[i]);
  if(size == -1){
    fprintf(stderr, "Hata: %s dosyası bulunamadı veya okunamadı.\n", input_files[i]);
    exit(1);
  }
  
  total_size += size;
  if(total_size > MAX_TOTAL_SIZE){
    fprintf(stderr,"Hata: giriş dosyalarının toplam boyutu 200 MB'ı aşamaz!\n");
    exit(1); //hatalı çıkış
  }
  
    char perm[10];
    if(!get_file_permissions(input_files[i], perm)){
      fprintf(stderr,"Hata: %s dosyasının izinleri okunamadı.\n", input_files[i]);
      exit(1);
    }

    snprintf(temp_buf, sizeof(temp_buf), "|%s,%s,%ld|", input_files[i], perm, size); //istenen format |dosya adı, izinlerr, boyut|
    
    if(strlen(metadata) + strlen(temp_buf) + 1 > META_BUF_SIZE){
      fprintf(stderr,"Hata: metadata tamponu taştı.\n");
      exit(1);
    }
    strcat(metadata, temp_buf);
  }
  
  FILE *out = fopen(output_file, "wb");
  if(!out){
    fprintf(stderr,"Hata: %s arşiv dosyası oluşturulamadı!\n", output_file);
    exit(1);
  }
  
  fprintf(out, "%010zu", strlen(metadata)); //örnek çıktı: 0000000142
  
  fwrite(metadata, 1, strlen(metadata), out);
  
  for(int i = 0; i < count; i++){
    FILE *in = fopen(input_files[i], "rb");
    if(in){
      int ch;
      while ((ch = fgetc(in)) != EOF){
        fputc(ch, out);
      }
      fclose(in);
    } else {
      fprintf(stderr,"Hata: %s dosyası açılamadı.\n", input_files[i]);
      fclose(out);
      exit(1);
    }
  }
  fclose(out);
  printf("dosyalar birleştirildi.\n");
}

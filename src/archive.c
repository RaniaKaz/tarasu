#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/archive.h"
#include "../include/utils.h"

#define MAX_TOTAL_SIZE (200 * 1024 * 1024) //200 MB

void archive_files(char *input_files[], int count, const char output_files){
  long total_size = 0;
  char metadata[8192] = ""; //32 dosyanın bilgisi için
  char temp_buf[512];
  
  //dosya doğrulama ve metadata hazırlığı
  for(int i = 0; i < count; i++){
    if(!is_text_file(input_files[])){
      printf("%s Giriş dosyasının formatı uyumsuzdur!\n", input_file[i]);
      exit(0); //başarılı çıkış
  }
  
  long size = get_file_size(input_files[i]);
  if(size == -1){
    printf("Hata: %s dosyası bulunamadı veya okunamadı");
    exit(1);
  }
  
  total_size += size;
  if(total_size > MAX_TOTAL_SIZE){
    printf("Hata: giriş dosyalarının toplam boyutu 200 MB'ı aşamaz!");
    exit(1); //hatalı çıkış
  }
  
  char perm[10];
  get_file_permissions(input_files[i], perm);
  sprintf(temp_buf, "|%s,%s,%ld|", input_files[i], perm, size); //istenen format |dosya adı, izinlerr, boyut|
  strcat(metadata, temp_buf);
  }
  
  FILE *out = fopen(output_file, "w");
  if(!out){
    printf("Hata: arşiv dosyası oluşturulamadı!\n");
    exit(1);
  }
  fprintf(out, "%010zu", strlen(metadata)); //örnek çıktı: 0000000142
  fprintf(out, "%s", metadata);
  
  for(int i = 0; i < count; i++){
    FILE *in = openf(input_files[i], "r");
    if(in){
      int ch;
      while ((ch = fgetc(in) != EOF){
        fputc(ch, out);
      }
      fclose(in);
    }
  }
  fclose(out);
  printf("dosyalar birleştirildi.\n");
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include "../include/extract.h"

#define MAX_FILES 32

// Çok seviyeli dizin oluşturma
static void make_dirs(const char *path) {
  char tmp[512];
  strncpy(tmp, path, sizeof(tmp) - 1);
  tmp[sizeof(tmp) - 1] = '\0';
  size_t len = strlen(tmp);

  if (len > 0 && tmp[len -1] == '/') tmp[--len] = '\0';

  for (size_t i = 1; i <= len; i++) {
    if (tmp[i] == '/' || tmp[i] == '\0'){
      char save = tmp[i];
      tmp[i] = '\0';
      if (mkdir(tmp, 0777) != 0 && errno != EEXIST){
        fprintf(stderr,"Hata: %s dizini oluşturulamadı: %s\n", tmp, strerror(errno));
        exit(1);
      }
      tmp[i] =save;
    }
  }
}

void extract_files(const char *archive_file, const char *output_dir){
  FILE *in = fopen(archive_file, "rb");
  if(!in){
    printf("Arşiv dosyası uygunsuz veya bozuk!");
    exit(1);
  }
  
  //metadata uzunluğu ilk 10 byte oku
  char header_len_str[11];
  //okunan verinin kaydedileceği yer, boyut, kaç adet, verinin okunacağı açık olan dosya
  if(fread(header_len_str, 1, 10, in) != 10){ //başarıyla okuduğu veri sayısı döndürür
    printf("Arşiv dosyası uygunsuz veya bozuk!");
    fclose(in);
    exit(1);
  }
  header_len_str[10] = '\0';
  
  // Sadece rakamlardan oluşmalı kontrolü eklendi
  for (int i = 0; i < 10; i++) {
    if (header_len_str[i] < '0' || header_len_str[i] > '9') {
      printf("Arşiv dosyası uygunsuz veya bozuk!");
      fclose(in);
      exit(1);
    }
  }

  long header_len = atol(header_len_str);
  if(header_len <= 0){
    printf("Arşiv dosyası uygunsuz veya bozuk!");
    fclose(in);
    exit(1);
  }
  
  //metadatayı belleğe al
  char* metadata = malloc(header_len +1);
  if(!metadata){ // bellek tahsisi kontrolü eklendi
    fprintf(stderr, "Hata: bellek tahsisi başarısız.\n");
    fclose(in);
    exit(1);
  }
  if((long)fread(metadata, 1, header_len, in) != header_len){
    printf("Arşiv dosyası uygunsuz veya bozuk!");
    fclose(in);
    free(metadata);
    exit(1);
  }
  metadata[header_len] = '\0';
  
  //geçerli dizin değilse hedef dizini oluştur
  if(strcmp(output_dir, ".") != 0){ //aynı dizin
    make_dirs(output_dir); // mkdir yerine çok seviyeli klasör açıcı kullanıldı
  }
  
  //metdatayı ayrıştır ve dosyaları oluştur
  char file_names[MAX_FILES][256]; //en fazla 32 dosya
  int file_count=0;
  
  //|dosya adı, izinler, boyut|
  char *meta_copy = malloc(header_len + 1);
  if (!meta_copy) {
      fprintf(stderr, "Hata: bellek tahsisi başarısız.\n");
      fclose(in);
      free(metadata);
      exit(1);
  }
  memcpy(meta_copy, metadata, header_len + 1);
  free(metadata);
  
  char *token = strtok(meta_copy, "|");
  while(token != NULL && file_count < 32){ // Maksimum 32 dosya sınırı eklendi
    char file_name[256];
    char perm[10];
    long size;
    //virgüllerden parçala
    if(sscanf(token, "%255[^,],%9[^,],%ld", file_name, perm, &size) != 3){
      token = strtok(NULL, "|");
      continue; // Bozuk formattaki kısmı atla
    } //size'ın başlangıç adresi lazım bize
    
    char file_path[768];
    if(strcmp(output_dir, ".") == 0){
      snprintf(file_path, sizeof(file_path), "%s", file_name); // Güvenlik için sprintf yerine snprintf
    }
    else{
      snprintf(file_path, sizeof(file_path), "%s/%s", output_dir, file_name); // Güvenlik için sprintf yerine snprintf
    }
    
    FILE *out = fopen(file_path, "wb");
    if(!out){
      printf("Hata: %s dosyası oluşturulamadı!\n", file_path);
      fclose(in);
      free(meta_copy);
      exit(1);
    }
    
    //içeriği booyut kadar oku ve yaz
    for(long i = 0; i < size; i++){
      int ch = fgetc(in);
      if(ch == EOF) break;
      fputc(ch, out); //c'deki harfler aslında rakam olarak tutulur
    }
    fclose(out);
    
    //izinleri geri yükle (string'ten octal'a)
    int perm_octal = (int)strtol(perm, NULL, 8); //null olana kadar oku ve 8'lik tabanına çevir
    chmod(file_path, perm_octal); //bu sistem çağrısı sayesinde işletim sisteminin dosya tablosundaki izin bitleri değiştirir.
    strncpy(file_names[file_count], file_name, 255);
    file_names[file_count][255] = '\0';
    file_count++;
    token = strtok(NULL, "|"); //orijinal metinde kaldığı yerden bir sonraki dosyayı keser
  }
  free(meta_copy);
  fclose(in);
  
  if (file_count == 0) { // Hiç dosya çıkarılamadıysa hata ver
    printf("Arşiv dosyası uygunsuz veya bozuk!\n");
    exit(1);
  }

  //istenen formattaki çıktıyı yazdır
  printf("%s dizininde ", output_dir);
  for(int i = 0; i < file_count; i++){
    printf("%s", file_names[i]);
    if(file_count > 1){ // Sadece 1 dosya varsa virgül atmaması için eklendi
        if(i<file_count -2) printf(", ");
        else if(i == file_count-2) printf(" ve ");
    }
  }
  printf(" dosyaları açıldı.\n");
}

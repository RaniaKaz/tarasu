#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <../include/extract.h>

void extract_files(const char *arcive_file, const char *output_dir){
  FILE *in = fopen(archive_file, "r");
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
  header_len_str[10] = 0;
  int header_len = atoi(header_len_str);
  if(header_len <= 0){
    printf("Arşiv dosyası uygunsuz veya bozuk!");
    fclose(in);
    exit(1);
  }
  
  //metadatayı belleğe al
  char* metadata = malloc(header_len +1);
  if(fread(metadata, 1, header_len, in) != header_len){
    printf("Arşiv dosyası uygunsuz veya bozuk!");
    fclose(in);
    free(metadata);
    exit(1);
  }
  meadata[header_len] = '\0';
  
  //geçerli dizin değilse hedef dizini oluştur
  if(strcmp(output_dir, ".") != 0){ //aynı dizin
    mkdir(output_dir, 0777);
  }
  
  //metdatayı ayrıştır ve dosyaları oluştur
  char file_names[32][256]; //en fazla 32 dosya
  int file_count=0;
  
  //|dosya adı, izinler, boyut|
  char *token = strtok(metadata, "|");
  while(token != NULL){
    char file_name[256];
    char perm[10];
    long size;
    //virgüllerden parçala
    sscanf(token, "%[^,],%[^,],%ld", file_name, perm, &size); //size'ın başlangıç adresi lazım bize
    
    char file_path[512];
    if(strcmp(output_dir, ".") == 0){
      sprintf(file_path, "%s", file_name);
    }
    else{
      sprintf(file_path, "%s/%s", output_dir, file_name);
    }
    
    FILE *out = fopen(file_name, "w");
    if(!out){
      printf("Hata: %s dosyası oluşturulamadı!\n", file_path);
      exit(1);
    }
    
    //içeriği booyut kadar oku ve yaz
    for(long i = 0; i < size; i++){
      int ch = fgetc(in)
      if(ch == EOF) break;
      fputc(ch, out); //c'deki harfler aslında rakam olarak tutulur
    }
    fclose(out);
    
    //izinleri geri yükle (string'ten octal'a)
    int perm_octal = (int)strtol(perm, NULL, 8); //null olana kadar oku ve 8'lik tabanına çevir
    chmod(file_path, perm_octal); //bu sistem çağrısı sayesinde işletim sisteminin dosya tablosundaki izin bitleri değiştirir.
    strcpy(file_names[file_count], file_name);
    file_count++;
    token = strtok(NULL, "|"); //orijinal metinde kaldığı yerden bir sonraki dosyayı keser
  }
  free(metadata);
  fclose(in);
  
  //istenen formattaki çıktıyı yazdır
  printf("%s dizininde", output_dir);
  for(int i = 0; i < file_count; i++){
    printf("%s", file_names[i]);
    if(i<file_count -2) printf(", ");
    else if(i == file_count-2) printf(" ve ");
  }
  printf(" dosyaları açıldı.\n");
}

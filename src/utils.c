#include <stdio.h>
#include <sys/stat.h>
#include <../include/utils.h>

//dosya boyutunu byte cinsinden dödürür
long get_file_size(const char *filename){
  struct stat st;
  if(stat(filename, &st) == 0){
    return st.st_size;
  }
  return -1; //dosya okunamadı
}

int is_text_file(const char *filename){
  FILE *file = fopen(filename, "r"); //read mode
  if(!file) return 0;
  
  //Dosyadan bir karakter alır, onu bir sayıya (ASCII kodu) dönüştürür ve okuma imlecini bir adım ileri taşır.
  //Başarılı olursa karakterin değerini, dosya bittiyse veya hata oluştuysa EOF (End Of File) değerini döndürür.
  int ch; 
  while((ch = fgetc(file) != EOF){
  // Genişletilmiş ASCII (>127) veya kontrol karakterleri (tab, newline hariç) varsa metin değildir
    if(ch > 127 || (ch < 32 && ch != '\n' && ch != '\r' && ch != '\t')){
      fclose(file);
      return 0; //uyumsuz format
    }
  }
  fclose(file);
  return 1; //uyumlu format
}

//dosya izinleri octal formatta (örneğin 0654) alır
int get_file_permissions(const char *filename, char *perm_str){
  struc stat st;
  if(stat(filename, &st) == 0){
    //sadece son 3 basamağı (rwx) maskeleyip strng'e çeviriyoruz
    sprintf(perm_str, "%04o", st.st_mode & 0777); //ilk paramtre pointer alır
    return 1;
  }
  return 0;
}

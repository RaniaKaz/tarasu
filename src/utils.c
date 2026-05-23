#include <stdio.h>
#include <sys/stat.h>
#include "../include/utils.h"

//dosya boyutunu byte cinsinden dödürür
long get_file_size(const char *filename){
  struct stat st;
  if(stat(filename, &st) == 0){
    return st.st_size;
  }
  return -1; //dosya okunamadı
}

int is_text_file(const char *filename){
  FILE *file = fopen(filename, "rb"); //read mode
  if(!file) return 0;
  
  unsigned char buf[4096];
  size_t n;
    while ((n = fread(buf, 1, sizeof(buf), file)) > 0) {
        size_t i = 0;
        while (i < n) {
            unsigned char c = buf[i];

            //ASCII aralığı 
            if (c < 0x80) {
                //Kontrol karakterleri: sadece \t \n \r izinli
                if (c < 0x20 && c != '\t' && c != '\n' && c != '\r') {
                    fclose(file);
                    return 0;
                }
                i++;
                continue;
            }

            ///UTF-8 çok baytlı sekans başı 
            int extra = 0;
            if      ((c & 0xE0) == 0xC0) extra = 1; /* 2 bayt */
            else if ((c & 0xF0) == 0xE0) extra = 2; /* 3 bayt */
            else if ((c & 0xF8) == 0xF0) extra = 3; /* 4 bayt */
            else {
                /* 0x80–0xBF: beklenmedik devam baytı → binary */
                fclose(file);
                return 0;
            }

            /* Devam baytlarını kontrol et (10xxxxxx olmalı) */
            for (int j = 1; j <= extra; j++) {
                if (i + j >= n) {
                    /* Sekans tampon sınırına denk geldi; dosyadan devamını oku */
                    unsigned char cb;
                    if (fread(&cb, 1, 1, file) != 1 || (cb & 0xC0) != 0x80) {
                        fclose(file);
                        return 0;
                    }
                } else {
                    if ((buf[i + j] & 0xC0) != 0x80) {
                        fclose(file);
                        return 0;
                    }
                }
            }
            i += extra + 1;
        }
    }

    fclose(file);
    return 1;
}

//dosya izinleri octal formatta (örneğin 0654) alır
int get_file_permissions(const char *filename, char *perm_str){
  struct stat st;
  if(stat(filename, &st) == 0){
    //sadece son 3 basamağı (rwx) maskeleyip strng'e çeviriyoruz
    sprintf(perm_str, "%04o", st.st_mode & 0777); //ilk paramtre pointer alır
    return 1;
  }
  return 0;
}

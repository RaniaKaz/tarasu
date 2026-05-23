#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/archive.h"
#include "../include/extract.h"

#define MAX_FILES 32

int main(int argc, char *argv[]){
  //en az 3 parametre olmalı
  if(argc < 3){
            fprintf(stderr,
            "Kullanım:\n"
            "  tarsau -b <dosya1> [dosya2 ...] [-o arsiv.sau]\n"
            "  tarsau -a <arsiv.sau> [hedef_dizin]\n");
    return 1;
  }
  
  //arşivleme işlemi
  if(strcmp(argv[1], "-b") == 0){
    char *output_file = "a.sau"; //varsayılan arşiv adı
    char *input_files[MAX_FILES]; //max 32 dosya
    int input_count= 0;
    
    for(int i =2; i<argc; i++){
      if(strcmp(argv[i], "-o") ==0){
        if(i+1 < argc){
          output_file= argv[i+1];
          i++;
        }
        else{
          fprintf(stderr,"Hata: -o parametresinden sonra arşiv adı belitilmeli.\n");
          return 1;
        }
      }
      else{ //giriş dosyaları almaya devam ediyor
        if(input_count >= MAX_FILES){
          fprintf(stderr, "Hata: en fazla %d giriş dosyası desteklenir.\n", MAX_FILES);
          return 1;
        }
        input_files[input_count++] = argv[i];
      }
    }
    if(input_count == 0){
      fprintf(stderr, "Hata: Arşivlenecek dosya belirtilmeli.\n");
      return 1;
    }
    archive_files(input_files, input_count, output_file);
  }
  else if(strcmp(argv[1], "-a") == 0){
    if (argc > 4) {
        fprintf(stderr, "Hata: -a parametresinden sonra en fazla 2 parametre alınabilir.\n");
        return 1;
    }
    char *archive_file = argv[2];
        const char *output_dir   = (argc >= 4) ? argv[3] : ".";

        /* .sau uzantısı kontrolü */
        size_t len = strlen(archive_file);
        if (len < 4 || strcmp(archive_file + len - 4, ".sau") != 0) {
            printf("Arşiv dosyası uygunsuz veya bozuk!\n");
            return 1;
        }

        extract_files(archive_file, output_dir);

    } else {
        fprintf(stderr,
            "Hata: geçersiz parametre '%s'. -b veya -a kullanın.\n", argv[1]);
        return 1;
    }

    return 0;
}

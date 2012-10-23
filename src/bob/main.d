main.o main.d : main.c ../include/iface.h ../include/spi.h ../include/lcd.h \
  ../include/aes.h ../include/aes_types.h ../include/aes128_enc.h \
  ../include/aes_enc.h ../include/aes192_enc.h ../include/aes256_enc.h \
  ../include/aes128_dec.h ../include/aes_dec.h ../include/aes192_dec.h \
  ../include/aes256_dec.h ../include/aes_keyschedule.h

#ifndef DEU_AES_H_
#define DEU_AES_H_

void secure_decrypt(u8 *key, u8 *iv, u8 *src, u8 *dst, u32 nbytes);
void secure_encrypt(u8 *key, u8 *iv, u8 *src, u8 *dst, u32 nbytes);

#endif

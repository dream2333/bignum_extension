#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "bn.h"

struct bn
{
  int bn_len;
  int bn_size;
  int bn_sign;
  uint16_t *bn_data;
};

static int bn_resize(bn_t bn, int size)
{
  if (size <= bn->bn_size)
    return 0;
  uint16_t *data = (uint16_t *)realloc(bn->bn_data, size * sizeof(uint16_t));
  if (data == NULL)
    return -1;
  for (int i = bn->bn_size; i < size; i++)
    data[i] = 0;
  bn->bn_data = data;
  bn->bn_size = size;
  return 1;
}

static int bn_reallen(bn_t bn)
{
  int l = bn->bn_len;
  while (l-- > 0)
  {
    if (bn->bn_data[l] != 0)
      return l + 1;
  }
  return 0;
}

static void dbn_push(bn_t bn, uint8_t data)
{
  uint32_t carry = data;
  for (int j = 0; j < bn->bn_len; j++)
  {
    carry += bn->bn_data[j] * 256;
    bn->bn_data[j] = carry % 10000;
    carry = carry / 10000;
  }
  if (carry != 0)
    bn->bn_data[bn->bn_len++] = carry;
}

static bn_t todec(bn_t bn)
{
  int binlen = bn_reallen(bn);
  int declen = ((binlen + 3) / 4) * 5;
  bn_t dbn = bn_alloc();
  if (dbn == NULL)
    return NULL;
  bn_resize(dbn, declen);
  for (int i = binlen; i--;)
  {
    dbn_push(dbn, bn->bn_data[i] >> 8);
    dbn_push(dbn, bn->bn_data[i] & 0xFF);
  }
  return dbn;
}

bn_t bn_alloc(void)
{
  bn_t bn = (bn_t)malloc(sizeof(struct bn));
  if (bn == NULL)
    return NULL;
  bn->bn_data = (uint16_t *)calloc(1, sizeof(uint16_t));
  if (bn->bn_data == NULL)
  {
    free(bn);
    return NULL;
  }
  bn->bn_len = 0;
  bn->bn_size = 1;
  bn->bn_sign = 1;
  return bn;
}

int bn_toString(bn_t bn, char *buf, int buflen)
{
  bn_t dbn = todec(bn);
  if (dbn == NULL)
    return -1;
  int dlen = dbn->bn_len;
  uint16_t *data = dbn->bn_data;

  int requiredlen;
  if (dlen == 0)
    requiredlen = 2;
  else
    requiredlen = 2 + (bn->bn_sign < 0) + (dlen - 1) * 4 +
                  (data[dlen - 1] > 999) +
                  (data[dlen - 1] > 99) +
                  (data[dlen - 1] > 9);
  if (requiredlen > buflen)
  {
    bn_free(dbn);
    return requiredlen;
  }

  char *p = buf;

  if (dlen == 0)
  {
    *p++ = '0';
  }
  else
  {
    if (bn->bn_sign < 0)
      *p++ = '-';
    dlen--;
    if (data[dlen] > 999)
      *p++ = '0' + (data[dlen] / 1000) % 10;
    if (data[dlen] > 99)
      *p++ = '0' + (data[dlen] / 100) % 10;
    if (data[dlen] > 9)
      *p++ = '0' + (data[dlen] / 10) % 10;
    *p++ = '0' + data[dlen] % 10;
    while (dlen--)
    {
      *p++ = '0' + (data[dlen] / 1000) % 10;
      *p++ = '0' + (data[dlen] / 100) % 10;
      *p++ = '0' + (data[dlen] / 10) % 10;
      *p++ = '0' + (data[dlen] / 1) % 10;
    }
  }
  *p = '\0';
  bn_free(dbn);
  return 0;
}

int main()
{
  return 0;
}
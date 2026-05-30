/****************************************************************************
 * apps/testing/libc/popen/popen_test.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define POPEN_TEST_BUFLEN 64

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int popen_test_read(FAR FILE *stream, FAR const char *expected)
{
  char buffer[POPEN_TEST_BUFLEN];

  if (fgets(buffer, sizeof(buffer), stream) == NULL)
    {
      printf("popen_test: fgets failed: %d\n", errno);
      return -1;
    }

  if (strcmp(buffer, expected) != 0)
    {
      printf("popen_test: got \"%s\", expected \"%s\"\n", buffer,
             expected);
      return -1;
    }

  return 0;
}

static int popen_test_close(FAR FILE *stream)
{
  int ret;

  ret = pclose(stream);
  if (ret < 0 && errno != ECHILD)
    {
      printf("popen_test: pclose failed: %d\n", errno);
      return -1;
    }

  return 0;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: popen_test_main
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  FAR FILE *stream;
  FAR FILE *first;
  FAR FILE *second;

  first = popen("echo popen_test_first", "r");
  if (first == NULL)
    {
      printf("popen_test: first popen failed: %d\n", errno);
      return EXIT_FAILURE;
    }

  second = popen("echo popen_test_second", "r");
  if (second == NULL)
    {
      printf("popen_test: second popen failed: %d\n", errno);
      popen_test_close(first);
      return EXIT_FAILURE;
    }

  if (popen_test_read(first, "popen_test_first\n") < 0)
    {
      goto out_with_second;
    }

  stream = first;
  first = NULL;
  if (popen_test_close(stream) < 0)
    {
      goto out_with_second;
    }

  if (popen_test_read(second, "popen_test_second\n") < 0)
    {
      goto out_with_second;
    }

  stream = second;
  second = NULL;
  if (popen_test_close(stream) < 0)
    {
      return EXIT_FAILURE;
    }

  printf("popen_test: successful\n");
  return EXIT_SUCCESS;

out_with_second:
  if (second != NULL)
    {
      popen_test_close(second);
    }

  if (first != NULL)
    {
      popen_test_close(first);
    }

  return EXIT_FAILURE;
}

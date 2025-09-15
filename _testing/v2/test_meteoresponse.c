#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

/*
 * Resources:
 * https://curl.se/libcurl/c/CURLOPT_WRITEFUNCTION.html
 * https://stackoverflow.com/questions/13905774/in-c-how-do-you-use-libcurl-to-read-a-http-response-into-a-string?rq=3
 * https://curl.se/libcurl/c/getinmemory.html
 *
*/

struct memory_chunk {
  char *addr;  // Pointer to adress for chunk
  size_t size; // Size of chunk
};

// Mystery daniel@haxx.se callback function
static size_t write_memory(void *contents, size_t size, size_t nmemb, struct memory_chunk *data) // What is nmemb?
{
  size_t realsize = size * nmemb; 
  struct memory_chunk *mem = (struct memory_chunk *)data; // What the hell does this mean? We define a new chunk off the input chunk?

  char *ptr = realloc(mem->addr, mem->size + realsize + 1); // We reallocate memory for our chunk and make a pointer to the new addr
  if (!ptr)
  {
    printf("Not enough memory - realloc returned NULL\n");
    return 0;
  }

  mem->addr = ptr; // We redefine our addr to the pointer since realloc went well
  memcpy(&(mem->addr[mem->size]), contents, realsize); // We copy realsize*bytes from contents to our chunk
  mem->size += realsize; // we add realsize to our chunksize
  mem->addr[mem->size] = 0; // We set the last byte to zero, because?
  
  return realsize; // We return the size of the chunk...
}


char* get_meteo_response(char* url, char* response)
{

  CURL *curl;
  CURLcode res;
  struct memory_chunk data;

  data.addr = malloc(1); // We allocate a memory address to our data struct
  data.size = 0; // We will reallocate memory to it in write_memory

  curl_global_init(CURL_GLOBAL_DEFAULT); //init curl with defaults (same as _ALL = WIN32 && SSL)
  curl = curl_easy_init();

  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_memory);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&data);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "TempleOSExplorer/1.0 (TempleBot/16.0; HolyCScript) DivineEngine/20231220");

  printf("Getting response...\n");
  res = curl_easy_perform(curl);

  if (res != CURLE_OK) // If response code is not ok we return the error response from api
  {
    #ifdef DEBUG
    printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    #endif

    strncpy(response, curl_easy_strerror(res), 512);
  }
  else // Else we copy our processed chunk memory to our response variable
  {
    #ifdef DEBUG
    printf("Response addr:\n%s\n", data.addr);
    printf("Response size:\n%lu\n", (unsigned long)data.size);
    #endif

    strncpy(response, data.addr, 512);
  }
  
  // We are done, clean up curl and free the data from memory 🐦
  curl_easy_cleanup(curl);
  curl_global_cleanup();
  free(data.addr);

  return response;
}

int main(void)
{
  char* url = "https://api.open-meteo.com/v1/forecast?latitude=59.3293&longitude=18.0686&current_weather=true";
  char response[512];
  get_meteo_response(url, response);
}

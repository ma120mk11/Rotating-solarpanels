/*  v1.0
 *  HTTP over TLS (HTTPS) sketch
 *
 *  This example demonstrates how to use
 *  client secure connect to access HTTPS API.
 *  This sketch uploads the data to Google Docs.
 *  The link for Google Spreadsheet:
 *  https://docs.google.com/spreadsheets/d/1gXEi51ec1EhLaPkwQn9Iorw4NKVxetNzNAsS8LYtLmg/edit#gid=0
 *  
 */
 
// Run in setup:
void wifi_INIT();

// Sends 6 values to google spreadsheet:
void logToCloud(float, float, float, float, float, float);

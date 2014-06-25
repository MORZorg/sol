#ifndef __DATA_IO_H__
#define __DATA_IO_H__

#include <Python.h>

const char* PYTHON_MODULE_NAME = "interface";
const char* PYTHON_REQUEST_INPUT_NAME = "requestInput";
const char* PYTHON_REQUEST_OUTPUT_NAME = "requestOutput";

/**
 * @brief Asks the user for data.
 *
 * @param char* The schema of the data to ask.
 *
 * @return The read data.
 */
char* userInput(char*);

/**
 * @brief Shows the user some data.
 *
 * @param char* The schema of the data to show.
 * @param char* The data to show.
 */
void userOutput(char*, char*);

/**
 * @brief Reads data to a file.
 *
 * @param char* The name of the file to read from.
 *
 * @return The read data.
 */
char* fileInput(char*);

/**
 * @brief Writes some data to a file.
 *
 * @param char* The name of the file to write to.
 * @param char* The data to write.
 */
void fileOutput(char*, char*);

#endif  // __DATA_IO_H__

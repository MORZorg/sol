#ifndef __DATA_IO_H__
#define __DATA_IO_H__

#include <Python.h>

#include "support_structures.h"

#define PYTHON_MODULE_NAME "interface"
#define PYTHON_CLASS_NAME "SimpleInterface"
#define PYTHON_REQUEST_INPUT_NAME "requestInput"
#define PYTHON_REQUEST_OUTPUT_NAME "requestOutput"

#define GUI_NO_INIT 0
#define GUI_EXT_INIT 1
#define GUI_SELF_INIT 2


/**
 * @brief Initialized the user interface variables.
 * This method can be called to make every subsequent call to the UI functions
 * faster.
 * If used, also finalize_gui() must be called before exiting the program.
 */
void initialize_gui(void);

/**
 * @brief Finalizes the user interface variables.
 * This method must only be called if initialize_gui() is called. Otherwise the
 * functions will handle it themselves.
 */
void finalize_gui(void);

/**
 * @brief Asks the user for data.
 *
 * @param char* The schema of the data to ask.
 *
 * @return The read data.
 */
ByteArray userInput(char*);

/**
 * @brief Shows the user some data.
 *
 * @param char* The schema of the data to show.
 * @param char* The data to show.
 */
void userOutput(char*, ByteArray);

/**
 * @brief Reads data to a file.
 *
 * @param char* The name of the file to read from.
 *
 * @return The read data.
 */
ByteArray fileInput(char*);

/**
 * @brief Writes some data to a file.
 *
 * @param char* The name of the file to write to.
 * @param char* The data to write.
 */
void fileOutput(char*, ByteArray);

#endif  // __DATA_IO_H__

#include "dataIO.h"

char* userInput(char* schema)
{
	PyObject* gui_module = PyImport_Import(PyString_FromString(PYTHON_MODULE_NAME));
	PyObject* gui_function = PyObject_GetAttrString(gui_module,
													PYTHON_REQUEST_INPUT_NAME);
	PyObject* gui_args = PyTuple_Pack(1, PyString_FromString(schema));

	return PyString_AsString(PyObject_CallObject(gui_function, gui_args));
}

void userOutput(char* schema, char* data)
{
	PyObject* gui_module = PyImport_Import(PyString_FromString(PYTHON_MODULE_NAME));
	PyObject* gui_function = PyObject_GetAttrString(gui_module,
													PYTHON_REQUEST_OUTPUT_NAME);
	PyObject* gui_args = PyTuple_Pack(2,
									  PyString_FromString(schema),
									  PyString_FromString(data));

	PyObject_CallObject(gui_function, gui_args);
}

char* fileInput(char* filename)
{
	FILE* file = fopen(filename, "r");

	if (file != NULL)
	{
		fseek(file, 0, SEEK_END);
		long file_size = ftell(file);
		rewind(file);

		char* result = malloc(file_size + 1);
		fread(result, file_size, 1, file);
		fclose(file);

		result[file_size] = 0;
		return result;
	}
	else
		return NULL;
}

void fileOutput(char* filename, char* data)
{
	FILE* file = fopen(filename, "w");

	if (file != NULL)
	{
		fputs(data, file);
		fclose(file);
	}
}

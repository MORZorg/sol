#include "dataIO.h"

ByteArray userInput(char* schema)
{
	PyObject* gui_module = PyImport_Import(PyString_FromString(PYTHON_MODULE_NAME));
	PyObject* gui_function = PyObject_GetAttrString(gui_module,
													PYTHON_REQUEST_INPUT_NAME);
	PyObject* gui_args = PyTuple_Pack(1, PyString_FromString(schema));

	ByteArray result;
	PyString_AsStringAndSize(
		PyObject_CallObject(gui_function, gui_args),
		&result.value,
		&result.size);
	return result;
}

void userOutput(char* schema, ByteArray data)
{
	PyObject* gui_module = PyImport_Import(PyString_FromString(PYTHON_MODULE_NAME));
	PyObject* gui_function = PyObject_GetAttrString(gui_module,
													PYTHON_REQUEST_OUTPUT_NAME);
	PyObject* gui_args =
		PyTuple_Pack(2,
					 PyString_FromString(schema),
					 PyString_FromStringAndSize(data.value, data.size));

	PyObject_CallObject(gui_function, gui_args);
}

ByteArray fileInput(char* filename)
{
	FILE* file = fopen(filename, "r");

	ByteArray result;
	if (file != NULL)
	{
		fseek(file, 0, SEEK_END);
		result.size = ftell(file);
		rewind(file);

		result.value = malloc(result.size + 1);
		fread(result.value, result.size, 1, file);
		fclose(file);

		result.value[result.size] = 0;
		return result;
	}
	else
	{
		result.size = 0;
		result.value = NULL;
		return result;
	}
}

void fileOutput(char* filename, ByteArray data)
{
	FILE* file = fopen(filename, "w");

	if (file != NULL)
	{
		fwrite(data.value, sizeof( byte ), data.size, file);
		fclose(file);
	}
}

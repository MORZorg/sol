#include "dataIO.h"

ByteArray userInput(char* schema)
{
	Py_Initialize();

	char *path, *new_path;
	char* added_path = ":/usr/local/lib/python2.7/site-packages:.";
	path = Py_GetPath();
	new_path = malloc(sizeof(char) * (strlen(path) + strlen(added_path) + 2));
	strcpy(new_path, path);
	strcat(new_path, added_path);  // ":." for unix, or ";." for windows
	PySys_SetPath(new_path);
	free(new_path);

	PyObject* gui_module = PyImport_Import(PyString_FromString(PYTHON_MODULE_NAME));
	PyObject* gui_function = PyObject_GetAttrString(gui_module,
													PYTHON_REQUEST_INPUT_NAME);
	PyObject* gui_args = PyTuple_Pack(1, PyString_FromString(schema));

	ByteArray result;
	PyString_AsStringAndSize(
		PyObject_CallObject(gui_function, gui_args),
		&result.value,
		&result.size);

	Py_Finalize();

	return result;
}

void userOutput(char* schema, ByteArray data)
{
	Py_Initialize();
	
	char *path, *new_path;
	char* added_path = ":/usr/local/lib/python2.7/site-packages:.";
	path = Py_GetPath();
	new_path = malloc(sizeof(char) * (strlen(path) + strlen(added_path) + 2));
	strcpy(new_path, path);
	strcat(new_path, added_path);  // ":." for unix, or ";." for windows
	PySys_SetPath(new_path);
	free(new_path);

	PyObject* gui_name = PyString_FromString(PYTHON_MODULE_NAME);
	PyObject* gui_module = PyImport_Import(PyString_FromString(PYTHON_MODULE_NAME));
	Py_DECREF(gui_name);

	PyObject* gui_function = PyObject_GetAttrString(gui_module,
													PYTHON_REQUEST_OUTPUT_NAME);
	PyObject* gui_args =
		PyTuple_Pack(2,
					 PyString_FromString(schema),
					 PyString_FromStringAndSize(data.value, data.size));

	PyObject_CallObject(gui_function, gui_args);

	Py_Finalize();
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
